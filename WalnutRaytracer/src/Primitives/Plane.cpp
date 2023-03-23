#include "Plane.h"

float Plane::Intersect(const Ray& ray) const
{
	//Both are normalized, so:
	float d = glm::dot(-m_Normal, ray.Direction); //Negate normal cause the directions should be -opposite-.
	
	if (d < 1e-6)
		return std::numeric_limits<float>::max();

	glm::vec3 OriginToNormal = m_Position - ray.Origin;
	float t = glm::dot(OriginToNormal, -m_Normal) / d;
	return t;
}