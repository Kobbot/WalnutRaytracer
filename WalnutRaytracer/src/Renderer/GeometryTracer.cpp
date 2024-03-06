#include "GeometryTracer.h"

HitPayload GeometryTracer::TraceRay(const Ray& ray, const Scene* scene)
{
	//uint8_t  r = (uint8_t)(coord.x * 255.0f);
	//uint8_t  g = (uint8_t)(coord.y * 255.0f);

	//float radius = 0.5f;
	int closestSphere = -1;
	float sphereHitDistance = std::numeric_limits<float>::max();

	for (size_t i = 0; i < scene->Spheres.size(); i++)
	{
		const Sphere& sphere = scene->Spheres[i];
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

	for (size_t i = 0; i < scene->Planes.size(); i++)
	{
		const Plane& plane = scene->Planes[i];
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

	for (size_t i = 0; i < scene->Triangles.size(); i++)
	{
		const Triangle& triangle = scene->Triangles[i];
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

	for (size_t i = 0; i < scene->SphereLights.size(); i++)
	{
		const SphereLight& sphLight = scene->SphereLights[i];
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



	if (closestPrimitive < 0 || objectType < 0)
		return MissShader(ray);//Second condition is mostly a soft security check

	return ClosestHit(ray, scene, primitiveHitDistance, closestPrimitive, objectType);
};

HitPayload GeometryTracer::ClosestHit(const Ray& ray, const Scene* scene, float hitDistance, int objectIndex, int objectType)
{

	HitPayload payload;
	payload.HitDistance = hitDistance;
	payload.ObjectIndex = objectIndex;
	payload.ObjectType = objectType;

	if (objectType == 1) {
		const Sphere& closestSphere = scene->Spheres[objectIndex];

		glm::vec3 origin = ray.Origin - closestSphere.GetPosition(); //Moving back to the origin
		glm::vec3 position = origin + ray.Direction * hitDistance;
		payload.WorldNormal = glm::normalize(position);
		payload.WorldPosition = position + closestSphere.GetPosition(); //Moving back to where the sphere was in world position
		payload.MaterialIndex = closestSphere.GetMaterialIndex();
		payload.ray = ray;
	}
	else if (objectType == 2) {
		const Plane& closestPlane = scene->Planes[objectIndex];

		glm::vec3 origin = ray.Origin - closestPlane.GetPosition(); //Moving back to the origin
		glm::vec3 position = origin + ray.Direction * hitDistance;
		payload.WorldNormal = closestPlane.GetNormal();
		payload.WorldPosition = position + closestPlane.GetPosition(); //Moving back to where the sphere was in world position
		payload.MaterialIndex = closestPlane.GetMaterialIndex();
		payload.ray = ray;
	}
	else if (objectType == 3) {
		const Triangle& closestTriangle = scene->Triangles[objectIndex];

		glm::vec3 origin = ray.Origin - closestTriangle.GetPosition(); //Moving back to the origin
		glm::vec3 position = origin + ray.Direction * hitDistance;
		payload.WorldNormal = closestTriangle.GetNormal();
		payload.WorldPosition = position + closestTriangle.GetPosition(); //Moving back to where the sphere was in world position
		payload.MaterialIndex = closestTriangle.GetMaterialIndex();
		payload.ray = ray;
	}
	else if (objectType == 11) {
		const SphereLight& closestSphLight = scene->SphereLights[objectIndex];

		glm::vec3 origin = ray.Origin - closestSphLight.Sphere.GetPosition(); //Moving back to the origin
		glm::vec3 position = origin + ray.Direction * hitDistance;
		payload.WorldNormal = glm::normalize(position);
		payload.WorldPosition = position + closestSphLight.Sphere.GetPosition(); //Moving back to where the sphere was in world position
		payload.MaterialIndex = closestSphLight.Sphere.GetMaterialIndex();
		payload.ray = ray;
	}

	return payload;
}

HitPayload GeometryTracer::MissShader(const Ray& ray)
{
	//Maybe set the object type to 0 for skybox here? Or do this a bit more useful?
	HitPayload payload;
	payload.HitDistance = -1.0f;
	return payload;
};