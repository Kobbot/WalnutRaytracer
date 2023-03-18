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
	m_ActiveScene = &scene;
	m_ActiveCamera = &camera;

	for (uint32_t y = 0; y <  m_FinalImage->GetHeight(); y++) {
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
			uint32_t index = x + y * m_FinalImage->GetWidth();
			glm::vec4 color = PerPixel(index);
			m_ImageData[index] = Utils::ColorVecToRGBA(color);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
	//uint8_t  r = (uint8_t)(coord.x * 255.0f);
	//uint8_t  g = (uint8_t)(coord.y * 255.0f);


	//float radius = 0.5f;
	int closestSphere = -1;
	float hitDistance = std::numeric_limits<float>::max();

	for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++)
	{
		const Sphere& sphere = m_ActiveScene->Spheres[i];
		float closestT = sphere.Intersect(ray);
		if (closestT > 0.0f && closestT < hitDistance)
		{
			hitDistance = closestT;
			closestSphere = (int)i;
		}

	}

	if (closestSphere < 0)
		return MissShader(ray);

	return ClosestHit(ray, hitDistance, closestSphere);

	// (bx^2 + by^2)t^2 + 2(axbx + ayby)t + (ax^2 + ay^2 - r^2) = 0	
};

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex)
{

	Renderer::HitPayload payload;
	payload.HitDistance = hitDistance;
	payload.ObjectIndex = objectIndex;

	const Sphere& closestSphere = m_ActiveScene->Spheres[objectIndex];

	glm::vec3 origin = ray.Origin - closestSphere.GetPosition(); //Moving back to the origin
	glm::vec3 position = origin + ray.Direction * hitDistance;
	payload.WorldNormal = glm::normalize(position);
	payload.WorldPosition = position + closestSphere.GetPosition(); //Moving back to where the sphere was in world position

	return payload;
}

Renderer::HitPayload Renderer::MissShader(const Ray& ray) 
{
	Renderer::HitPayload payload;
	payload.HitDistance = -1.0f;
	return payload;
};

glm::vec4 Renderer::PerPixel(uint32_t index) {
	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	ray.Direction = m_ActiveCamera->GetRayDirections()[index];

	glm::vec3 color(0.0f);
	float multiplier = 1.0f;

	int bounces = 2;
	for (int i = 0; i < bounces; i++) 
	{
		Renderer::HitPayload payload = TraceRay(ray);
		if (payload.HitDistance < 0.0f)
		{
			glm::vec3 skyColor = glm::vec3(0.6f, 0.7f, 0.9f);
			color += skyColor * multiplier;
			break;
		}

		glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
		float lighting = glm::max(glm::dot(payload.WorldNormal, -lightDir), 0.0f); // Equals the cosine between the normals

		const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];

		glm::vec3 sphereColor = sphere.GetMaterial().Albedo;
		sphereColor *= lighting;
		color += sphereColor * multiplier;
		multiplier *= 0.3f;

		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
		//Direction needs to be the direction reflected alongside the normal
		ray.Direction = glm::reflect(ray.Direction, 
			payload.WorldNormal + sphere.GetMaterial().Roughness * WN::Random::Vec3(-0.5f,0.5f));
	}

	return glm::vec4(color, 1.0f);
}
