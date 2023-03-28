#include "Plane.h"

float Plane::Intersect(const Ray& ray) const
{
	//Both are normalized, so:
	float denom = -glm::dot(m_Normal, ray.Direction);
	
	if (denom < 1e-6)
		return std::numeric_limits<float>::max();

	glm::vec3 OriginToNormal = m_Position - ray.Origin;
	float t = (glm::dot(OriginToNormal, -m_Normal)) / denom;
	return t;
}