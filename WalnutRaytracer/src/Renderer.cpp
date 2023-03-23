#include "Renderer.h"
#include "Walnut/Random.h"

#include <cmath>
#include <execution>

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

	delete[] m_AccumulationData;
	m_AccumulationData = new glm::vec4[width * height];

	m_ImageHorizontalIter.resize(width);
	m_ImageVerticalIter.resize(height);
	for (uint32_t i = 0; i < width; i++)
		m_ImageHorizontalIter[i] = i;

	for (uint32_t i = 0; i < height; i++)
		m_ImageVerticalIter[i] = i;

}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
	m_ActiveScene = &scene;
	m_ActiveCamera = &camera;

	if (m_FrameIndex == 1)
		memset(m_AccumulationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));


#define MT 1
#if MT

	std::for_each(std::execution::par, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(), 
		[this](uint32_t y) 
		{
			for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
				uint32_t index = x + y * m_FinalImage->GetWidth();

				glm::vec4 color = PerPixel(index);

				m_AccumulationData[index] += color; //This doesnt -need- a clamped color, but clamping happens earlier. Might be troublesome?

				glm::vec4 accumulatedColor = m_AccumulationData[index];
				accumulatedColor /= (float)m_FrameIndex;

				m_ImageData[index] = Utils::ColorVecToRGBA(accumulatedColor);
			}
		});

#else
	for (uint32_t y = 0; y <  m_FinalImage->GetHeight(); y++) {
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
			uint32_t index = x + y * m_FinalImage->GetWidth();

			glm::vec4 color = PerPixel(index);

			m_AccumulationData[index] += color; //This doesnt -need- a clamped color, but clamping happens earlier. Might be troublesome?

			glm::vec4 accumulatedColor = m_AccumulationData[index];
			accumulatedColor /= (float)m_FrameIndex;

			m_ImageData[index] = Utils::ColorVecToRGBA(accumulatedColor);
		}
	}
#endif

	m_FinalImage->SetData(m_ImageData);

	if (m_Settings.Accumulate)
		m_FrameIndex++;
	else
		m_FrameIndex = 1;
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
	//uint8_t  r = (uint8_t)(coord.x * 255.0f);
	//uint8_t  g = (uint8_t)(coord.y * 255.0f);


	//float radius = 0.5f;
	int closestSphere = -1;
	float sphereHitDistance = std::numeric_limits<float>::max();

	for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++)
	{
		const Sphere& sphere = m_ActiveScene->Spheres[i];
		float closestT = sphere.Intersect(ray);
		if (closestT > 0.0f && closestT < sphereHitDistance)
		{
			sphereHitDistance = closestT;
			closestSphere = (int)i;
		}

	}

	//Now we check planes.
	int closestPlane = -1;
	float planeHitDistance = std::numeric_limits<float>::max();

	for (size_t i = 0; i < m_ActiveScene->Planes.size(); i++)
	{
		const Plane&plane = m_ActiveScene->Planes[i];
		float t = plane.Intersect(ray);
		if (t > 0.0f && t < planeHitDistance)
		{
			planeHitDistance = t;
			closestPlane = (int)i;
		}
	}

	//TODO: More primitives here.

	int closestPrimitive = -1;
	float primitiveHitDistance = std::numeric_limits<float>::max();
	int objectType = -1;

	if (sphereHitDistance < primitiveHitDistance)
	{
		primitiveHitDistance = sphereHitDistance;
		closestPrimitive = closestSphere;
		objectType = 1; //Code for Sphere
	}
	if (planeHitDistance < primitiveHitDistance)
	{
		primitiveHitDistance = planeHitDistance;
		closestPrimitive = closestPlane;
		objectType = 2; //Code for Plane
	}


	if (closestPrimitive < 0 || objectType < 0) //Second condition is mostly a soft security check
		return MissShader(ray);

	return ClosestHit(ray, primitiveHitDistance, closestPrimitive, objectType);
};

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex, int objectType)
{

	Renderer::HitPayload payload;
	payload.HitDistance = hitDistance;
	payload.ObjectIndex = objectIndex;
	payload.ObjectType = objectType;

	if (objectType == 1) {
		const Sphere& closestSphere = m_ActiveScene->Spheres[objectIndex];

		glm::vec3 origin = ray.Origin - closestSphere.GetPosition(); //Moving back to the origin
		glm::vec3 position = origin + ray.Direction * hitDistance;
		payload.WorldNormal = glm::normalize(position);
		payload.WorldPosition = position + closestSphere.GetPosition(); //Moving back to where the sphere was in world position
		payload.MaterialIndex = closestSphere.GetMaterialIndex();
	}
	else if (objectType == 2) {
		const Plane& closestPlane = m_ActiveScene->Planes[objectIndex];

		glm::vec3 origin = ray.Origin - closestPlane.GetPosition(); //Moving back to the origin
		glm::vec3 position = origin + ray.Direction * hitDistance;
		payload.WorldNormal = closestPlane.GetNormal();
		payload.WorldPosition = position + closestPlane.GetPosition(); //Moving back to where the sphere was in world position
		payload.MaterialIndex = closestPlane.GetMaterialIndex();
	}
	else {
		//Something went VERY wrong
	}

	return payload;
}

Renderer::HitPayload Renderer::MissShader(const Ray& ray) 
{
	Renderer::HitPayload payload;
	payload.HitDistance = -1.0f;
	return payload;
};

//This function controls how many rays are cast per pixel and how they bounce
glm::vec4 Renderer::PerPixel(uint32_t index) {

	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	ray.Direction = m_ActiveCamera->GetRayDirections()[index];

	glm::vec3 color(0.0f);
	float multiplier = 1.0f;

	//Maximum number each ray 
	int bounces = 4;
	for (int i = 0; i < bounces; i++) 
	{
		Renderer::HitPayload payload = TraceRay(ray);
		if (payload.HitDistance < 0.0f)
		{
			glm::vec3 skyColor = glm::vec3(0.6f, 0.7f, 0.9f);
			color += skyColor * multiplier;
			break;
		}

		//Lightning is currently calculated through the angle between surface and light. Maybe should make this it's own function?
		glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
		float lighting = glm::max(glm::dot(payload.WorldNormal, -lightDir), 0.0f); // Equals the cosine between the normals

		//const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];

		const Material& mat = m_ActiveScene->Materials[payload.MaterialIndex];
		glm::vec3 matColor = mat.Albedo;

		//Add color to pixel, reduce multiplier
		matColor *= lighting;
		color += matColor * multiplier;
		multiplier *= 0.3f;

		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
		//Direction needs to be the direction reflected alongside the normal
		ray.Direction = glm::reflect(ray.Direction, 
			payload.WorldNormal + mat.Roughness * WN::Random::Vec3(-0.5f,0.5f));
	}

	return glm::vec4(color, 1.0f);
}
