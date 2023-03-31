#include "ShadowTracer.h"



float ShadowTracer::LightContribution(const Ray& surfaceNormal, const HitPayload& payload, const Scene* scene, GeometryTracer* tracer)
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

	glm::vec lightDir = scene->GlobalLight.Direction;
	float lightInt = scene->GlobalLight.Intensity;
	totalLight += lightInt;
	visibleLight += lightInt * glm::max(glm::dot(surfaceNormal.Direction, -lightDir), 0.0f);

	for (size_t i = 0; i < scene->PointLights.size(); i++)
	{
		const PointLight& pl = scene->PointLights[i];
		Ray shadowRay;
		shadowRay.Origin = surfaceNormal.Origin;
		shadowRay.Direction = glm::normalize(pl.Position - shadowRay.Origin);

		float distance = glm::distance(pl.Position, shadowRay.Origin);
		HitPayload payload = tracer->TraceRay(shadowRay, scene);

		float angle = glm::max(glm::dot(surfaceNormal.Direction, shadowRay.Direction), 0.0f);
		if (payload.HitDistance > 0.0f && distance < payload.HitDistance)
			visibleLight += (pl.Intensity / (distance + 0.000001)) * m_Settings->LightDecay * angle;  //The number 2 here is a decay offset (2 pow 1)
		//Also, squared is too abrupt a fall in sRGB, rather than linear color.
		//visibleLight += pl.Intensity * angle;
		//This also might not be necessary with Bidirectional Path Tracing
		totalLight += pl.Intensity;
	}

	for (size_t i = 0; i < scene->SphereLights.size(); i++)
	{

		//HARD SHADOWS METHOD

		if (!m_Settings->SoftShadows)
		{


			const SphereLight& sl = scene->SphereLights[i];
			//We'll YOINK the code from the poinlight for now.
			//TODO: Do this visibility with SAMPLING.
			Ray shadowRay;
			shadowRay.Origin = surfaceNormal.Origin;
			shadowRay.Direction = glm::normalize(sl.Sphere.GetPosition() - shadowRay.Origin);



			float distance = glm::distance(sl.Sphere.GetPosition(), shadowRay.Origin);
			HitPayload payload = tracer->TraceRay(shadowRay, scene);

			//We need to take into account that a colission will happen BEFORE the center of the sphere

			float angle = glm::max(glm::dot(surfaceNormal.Direction, shadowRay.Direction), 0.0f);
			if (payload.HitDistance > 0.0f && (distance - sl.Sphere.GetRadius() - 0.0001) < payload.HitDistance)
				visibleLight += (sl.Intensity / (distance - sl.Sphere.GetRadius() + 0.000001)) * m_Settings->LightDecay * angle;  //The number 2 here is a decay offset (2 pow 1
			totalLight += sl.Intensity;

		}
		else
		{
			//SOFT SHADOWS METHOD

			const SphereLight& sl = scene->SphereLights[i];
			//TODO: There's a better way to do this, and it's just sampling from a square perpendicular to the normal that points from the sphere to the fragment.
			//However I don't have the time to find this out, nor am I good enough at linear algebra for it.
			glm::vec3 p = sl.Sphere.GetPosition();
			float r = sl.Sphere.GetRadius();

			//So instead we'll find a point within the sphere.
			//Then raycast a ray. If the colission point is within the radius + epsilon distance of the center of the sphere, it's visible. Otherwise it isn't.


			//Why do more samples make the screen so dark. Is it cause of how accumulation works?
			int samples = m_Settings->ShadowSamples;
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

				HitPayload payload = tracer->TraceRay(shadowRay, scene);
				//There is a much easier way, actually! We can just check if we hit the sphere we are checking for right now!!


				if (payload.ObjectIndex == i && payload.ObjectType == 11)
				{
					float angle = glm::max(glm::dot(surfaceNormal.Direction, shadowRay.Direction), 0.0f);
					sampledLight += (sl.Intensity / (payload.HitDistance + 0.000001)) * m_Settings->LightDecay * angle;
				}
			}

			visibleLight += sampledLight;
			totalLight += sl.Intensity;
		}
	}

	if (payload.ObjectType == 11) {
		const SphereLight& sl = scene->SphereLights[payload.ObjectIndex];
		visibleLight += sl.Intensity * m_Settings->LightDecay;
	}
	//Adjust for if the object already is the SphereLight (It won't add itself since the angle will be more than 90 degrees to the normal).

	return visibleLight / totalLight;

}