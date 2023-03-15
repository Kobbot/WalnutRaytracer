#include "Sphere.h"

Sphere::Sphere(glm::vec3 position, glm::vec3 albedo, float radius)
	: Primitive(position, albedo), m_Radius(radius)
{}

float Sphere::Intersect(const Ray& ray) const {

	glm::vec3 origin = ray.Origin - m_Position;

	//Quadratic 
	// a = Ray Origin
	// b = Ray Direction
	// r = radius
	// t = hit distance

	float a = glm::dot(ray.Direction, ray.Direction);
	float b = 2.0f * glm::dot(origin, ray.Direction);
	float c = glm::dot(origin, origin) - m_Radius * m_Radius;

	// (-b +- sqrt(discriminant)) / (2.0f * a)

	float discriminant = b * b - 4.0f * a * c;

	if (discriminant < 0.0f)
		return std::numeric_limits<float>::max();

	//float tA = (-b + std::sqrt(discriminant)) / (2.0f * a);
	float closestT = (-b - std::sqrt(discriminant)) / (2.0f * a); //Smallest value, since we are subtracting

	return closestT;
	//if (closestT < hitDistance)
	//{
	//	hitDistance = closestT;
	//	closestSphere = &sphere;
	//}
}