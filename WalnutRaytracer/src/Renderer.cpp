#include "Renderer.h"
#include "Walnut/Random.h"

#include <cmath>

namespace WN = Walnut;

namespace Utils {

	static uint32_t ColorVecToRGBA(const glm::vec4& color)
	{
		//Clamp vector elements between 0 and 1
		glm::vec4 clampedColor = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));

		//Split elements into 8 bits each
		uint8_t r = (clampedColor.r * 255.0f);
		uint8_t g = (clampedColor.g * 255.0f);
		uint8_t b = (clampedColor.b * 255.0f);
		uint8_t a = (clampedColor.a * 255.0f);

		//Smush them together with the appropriate offset
		uint32_t result = ( (a << 24) | (b << 16) | (g << 8) | r );
		return result;
	}

}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		//If no resize necessary
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;
		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<WN::Image>(width, height, WN::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];

}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
	Ray ray;
	ray.Origin = camera.GetPosition();
	
	for (uint32_t y = 0; y <  m_FinalImage->GetHeight(); y++) {
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {

			uint32_t index = x + y * m_FinalImage->GetWidth();
			ray.Direction = camera.GetRayDirections()[index];

			glm::vec4 color = TraceRay(scene, ray);
			m_ImageData[index] = Utils::ColorVecToRGBA(color);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray)
{
	//uint8_t  r = (uint8_t)(coord.x * 255.0f);
	//uint8_t  g = (uint8_t)(coord.y * 255.0f);

	
	//float radius = 0.5f;


	if (scene.Spheres.size() == 0)
		return glm::vec4(0, 0, 0, 1);

	const Sphere* closestSphere = nullptr;
	float hitDistance = std::numeric_limits<float>::max();

	for (const Sphere& sphere : scene.Spheres)
	{
		glm::vec3 origin = ray.Origin - sphere.GetPosition();

		//Quadratic 
		// a = Ray Origin
		// b = Ray Direction
		// r = radius
		// t = hit distance

		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * glm::dot(origin, ray.Direction);
		float c = glm::dot(origin, origin) - sphere.GetRadius() * sphere.GetRadius();

		// (-b +- sqrt(discriminant)) / (2.0f * a)

		float discriminant = b * b - 4.0f * a * c;

		if (discriminant < 0.0f)
			continue;

		//float tA = (-b + std::sqrt(discriminant)) / (2.0f * a);
		float closestT = (-b - std::sqrt(discriminant)) / (2.0f * a); //Smallest value, since we are subtracting
		if (closestT < hitDistance)
		{
			hitDistance = closestT;
			closestSphere = &sphere;
		}

	}

	if (closestSphere == nullptr)
		return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	// (bx^2 + by^2)t^2 + 2(axbx + ayby)t + (ax^2 + ay^2 - r^2) = 0	
	glm::vec3 origin = ray.Origin - closestSphere->GetPosition();

	//glm::vec3 hitPositionA = origin + ray.Direction * tA;
	glm::vec3 hitPoint = origin + ray.Direction * hitDistance;

	//HitPosition - Origin (of the sphere i nthis case) is the normal. 
	//Therefore, cause the sphere is 0 ATM, HitPosition normalized IS the normal.

	glm::vec3 normal = glm::normalize(hitPoint);

	glm::vec3 lightDir = glm::normalize(glm::vec3(-1,-1,-1));

	float lighting = glm::max(glm::dot(normal, -lightDir), 0.0f); // Equals the cosine between the normals

	glm::vec3 sphereColor = closestSphere->GetAlbedo();
	sphereColor *= lighting;
	return glm::vec4(sphereColor, 1.0f);
}