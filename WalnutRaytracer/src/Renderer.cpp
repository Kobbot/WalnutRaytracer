#include "Renderer.h"
#include "Walnut/Random.h"

#include <cmath>
#include <execution>

namespace WN = Walnut;

#define lightDecay 1.5f

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

				if (m_Settings.LogAccumulation)
				{
					if (m_FrameIndex == 1) {
						m_AccumulationData[index] += color;
					}
					else
					{
						glm::vec4 colorDiff = color - m_AccumulationData[index];
						m_AccumulationData[index] += colorDiff * glm::max(glm::vec4(1 / glm::sqrt(m_FrameIndex)), 0.02f);
					}

					m_ImageData[index] = Utils::ColorVecToRGBA(m_AccumulationData[index]);
				}
				else
				{
					m_AccumulationData[index] += color; //This doesnt -need- a clamped color, but clamping happens earlier. Might be troublesome?

					glm::vec4 accumulatedColor = m_AccumulationData[index];
					accumulatedColor /= (float)m_FrameIndex;

					m_ImageData[index] = Utils::ColorVecToRGBA(accumulatedColor);
				}

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
		const Plane& plane = m_ActiveScene->Planes[i];
		float t = plane.Intersect(ray);
		if (t > 0.0f && t < planeHitDistance)
		{
			planeHitDistance = t;
			closestPlane = (int)i;
		}
	}

	//Now we check triangles.
	int closestTriangle = -1;
	float triangleHitDistance = std::numeric_limits<float>::max();

	for (size_t i = 0; i < m_ActiveScene->Triangles.size(); i++)
	{
		const Triangle& triangle = m_ActiveScene->Triangles[i];
		float t = triangle.Intersect(ray);
		if (t > 0.0f && t < triangleHitDistance)
		{
			triangleHitDistance = t;
			closestTriangle = (int)i;
		}
	}

	//Finally check Sphere Lights.
	int closestSphLight = -1;
	float sphLightHitDistance = std::numeric_limits<float>::max();

	for (size_t i = 0; i < m_ActiveScene->SphereLights.size(); i++)
	{
		const SphereLight& sphLight = m_ActiveScene->SphereLights[i];
		float t = sphLight.Sphere.Intersect(ray);
		if (t > 0.0f && t < sphLightHitDistance)
		{
			sphLightHitDistance = t;
			closestSphLight = (int)i;
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
	if (triangleHitDistance < primitiveHitDistance)
	{
		primitiveHitDistance = triangleHitDistance;
		closestPrimitive = closestTriangle;
		objectType = 3; //Code for Triangle
	}
	if (sphLightHitDistance < primitiveHitDistance)
	{
		primitiveHitDistance = sphLightHitDistance;
		closestPrimitive = closestSphLight;
		objectType = 11; //Code for Sphere light.
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
	else if (objectType == 3) {
		const Triangle& closestTriangle = m_ActiveScene->Triangles[objectIndex];

		glm::vec3 origin = ray.Origin - closestTriangle.GetPosition(); //Moving back to the origin
		glm::vec3 position = origin + ray.Direction * hitDistance;
		payload.WorldNormal = closestTriangle.GetNormal();
		payload.WorldPosition = position + closestTriangle.GetPosition(); //Moving back to where the sphere was in world position
		payload.MaterialIndex = closestTriangle.GetMaterialIndex();
	}
	else if (objectType == 11) {
		const SphereLight& closestSphLight = m_ActiveScene->SphereLights[objectIndex];

		glm::vec3 origin = ray.Origin - closestSphLight.Sphere.GetPosition(); //Moving back to the origin
		glm::vec3 position = origin + ray.Direction * hitDistance;
		payload.WorldNormal = glm::normalize(position);
		payload.WorldPosition = position + closestSphLight.Sphere.GetPosition(); //Moving back to where the sphere was in world position
		payload.MaterialIndex = closestSphLight.Sphere.GetMaterialIndex();
	}

	return payload;
}

Renderer::HitPayload Renderer::MissShader(const Ray& ray) 
{
	//Maybe set the object type to 0 for skybox here? Or do this a bit more useful?
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
			glm::vec3 skyColor = glm::vec3(0.2f, 0.0f, 0.4f);
			color += skyColor * multiplier;
			break;
		}

		const Material& mat = m_ActiveScene->Materials[payload.MaterialIndex];
		glm::vec3 matColor = mat.Albedo;

		Ray surfaceNormal;
		surfaceNormal.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
		surfaceNormal.Direction = payload.WorldNormal;

		//Lightning is currently calculated through the angle between surface and light. Maybe should make this it's own function?
		 // Equals the cosine between the normals

		//const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];

		//Add color to pixel, reduce multiplier
		if (m_Settings.GammaCorrection) {
			matColor.r = glm::sqrt(matColor.r);
			matColor.g = glm::sqrt(matColor.g);
			matColor.b = glm::sqrt(matColor.b);
		}

		matColor *= LightContribution(surfaceNormal, payload);
		color += matColor * multiplier;
		multiplier *= mat.Reflectivity; //Add reflectiveness?

		ray.Origin = surfaceNormal.Origin;
		//Direction needs to be the direction reflected alongside the normal
		//This is altering the normal to have a different direction, but the randomness is doing this in world space, not in reference to the normal.
		//This doesn't sit well with me. I should change it to something else. Maybe lambertian approximation?
		ray.Direction = glm::reflect(ray.Direction, 
			payload.WorldNormal + mat.Roughness * WN::Random::Vec3(-0.5f,0.5f));

	}


	return glm::vec4(color, 1.0f);
}

float Renderer::LightContribution(const Ray& surfaceNormal, const HitPayload& payload) 
{

	//Steps:
	//1. Calculate the contribution of light from global lighting.
	//2. Calculate the contribution of ligth from light sources.
	//2.1 Check visibility for each type of light source.
	//2.2 Multiply intensity by visibility.
	//2.3 Adjust by normal dot product.

	//glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
	//float lighting = glm::max(glm::dot(surfaceNormal.Direction, -lightDir), 0.0f);
	//return lighting;

	float totalLight = 0;
	float visibleLight = 0;

	glm::vec lightDir = m_ActiveScene->GlobalLight.Direction;
	float lightInt = m_ActiveScene->GlobalLight.Intensity;
	totalLight += lightInt;
	visibleLight += lightInt * glm::max(glm::dot(surfaceNormal.Direction, -lightDir), 0.0f);

	for (size_t i = 0; i < m_ActiveScene->PointLights.size(); i++)
	{
		const PointLight& pl = m_ActiveScene->PointLights[i];
		Ray shadowRay;
		shadowRay.Origin = surfaceNormal.Origin;
		shadowRay.Direction = glm::normalize(pl.Position - shadowRay.Origin);
		
		float distance = glm::distance(pl.Position, shadowRay.Origin);
		Renderer::HitPayload payload = TraceRay(shadowRay);

		float angle = glm::max(glm::dot(surfaceNormal.Direction, shadowRay.Direction), 0.0f);
		if (payload.HitDistance > 0.0f && distance < payload.HitDistance)
			visibleLight += (pl.Intensity / (distance + 0.000001)) * lightDecay * angle;  //The number 2 here is a decay offset (2 pow 1)
			//Also, squared is too abrupt a fall in sRGB, rather than linear color.
			//visibleLight += pl.Intensity * angle;
			//This also might not be necessary with Bidirectional Path Tracing
		totalLight += pl.Intensity;
	}

	for (size_t i = 0; i < m_ActiveScene->SphereLights.size(); i++)
	{

		//HARD SHADOWS METHOD
		
		if (!m_Settings.SoftShadows)
		{


			const SphereLight& sl = m_ActiveScene->SphereLights[i];
			//We'll YOINK the code from the poinlight for now.
			//TODO: Do this visibility with SAMPLING.
			Ray shadowRay;
			shadowRay.Origin = surfaceNormal.Origin;
			shadowRay.Direction = glm::normalize(sl.Sphere.GetPosition() - shadowRay.Origin);



			float distance = glm::distance(sl.Sphere.GetPosition(), shadowRay.Origin);
			Renderer::HitPayload payload = TraceRay(shadowRay);

			//We need to take into account that a colission will happen BEFORE the center of the sphere

			float angle = glm::max(glm::dot(surfaceNormal.Direction, shadowRay.Direction), 0.0f);
			if (payload.HitDistance > 0.0f && (distance - sl.Sphere.GetRadius() - 0.0001) < payload.HitDistance)
				visibleLight += (sl.Intensity / (distance - sl.Sphere.GetRadius() + 0.000001)) * lightDecay * angle;  //The number 2 here is a decay offset (2 pow 1
			totalLight += sl.Intensity;

		}
		else
		{
			//SOFT SHADOWS METHOD

			const SphereLight& sl = m_ActiveScene->SphereLights[i];
			//TODO: There's a better way to do this, and it's just sampling from a square perpendicular to the normal that points from the sphere to the fragment.
			//However I don't have the time to find this out, nor am I good enough at linear algebra for it.
			glm::vec3 p = sl.Sphere.GetPosition();
			float r = sl.Sphere.GetRadius();

			//So instead we'll find a point within the sphere.
			//Then raycast a ray. If the colission point is within the radius + epsilon distance of the center of the sphere, it's visible. Otherwise it isn't.


			//Why do more samples make the screen so dark. Is it cause of how accumulation works?
			int samples = m_Settings.ShadowSamples;
			float sampledLight = 0;
			for (int i = 0; i < samples; i++)
			{
				bool validSample = false;
				glm::vec3 sample;
				while (!validSample)
				{
					sample = p + WN::Random::Vec3(-r, r); //Random point within a cube containing the sphere.
					if (glm::distance(sample, p) <= (r + 0.0001))
						validSample = true;
				}
				//Now we raycast towards this point.
				Ray shadowRay;
				shadowRay.Origin = surfaceNormal.Origin;
				shadowRay.Direction = glm::normalize(sample - shadowRay.Origin);

				Renderer::HitPayload payload = TraceRay(shadowRay);
				//There is a much easier way, actually! We can just check if we hit the sphere we are checking for right now!!


				if (payload.ObjectIndex == i && payload.ObjectType == 11)
				{
					float angle = glm::max(glm::dot(surfaceNormal.Direction, shadowRay.Direction), 0.0f);
					sampledLight += (sl.Intensity / (payload.HitDistance + 0.000001)) * lightDecay * angle;
				}
			}

			visibleLight += sampledLight;
			totalLight += sl.Intensity;
		}
	}

	if (payload.ObjectType == 11) {
		const SphereLight& sl = m_ActiveScene->SphereLights[payload.ObjectIndex];
		visibleLight += sl.Intensity * lightDecay;
	}
	//Adjust for if the object already is the SphereLight (It won't add itself since the angle will be more than 90 degrees to the normal).

	return visibleLight / totalLight;

}