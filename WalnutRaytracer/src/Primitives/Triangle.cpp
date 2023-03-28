#include "Triangle.h"

float Triangle::Intersect(const Ray& ray) const
{
	
	//Yoinked from the plane intersection. Just to check if the ray and the plane of the triangle are colinear.
	float denom = -glm::dot(m_Normal, ray.Direction);

	if (denom < 1e-6)
		return std::numeric_limits<float>::max();

	//float distance = -glm::dot(m_Normal, m_p1);

	//Pretty much the same for now. Finding out the point of intersection with the plane.
	glm::vec3 OriginToNormal = m_p1 - ray.Origin;
	float t = glm::dot(OriginToNormal, -m_Normal) / denom;

	//Calculate the intersection point.
	glm::vec3 p = ray.Origin + (ray.Direction * t);

	//Now, we do the inside-outside test to see if it works.
	glm::vec3 edge1 = m_p2 - m_p1;
	glm::vec3 edge2 = m_p3 - m_p2;
	glm::vec3 edge3 = m_p1 - m_p3;

	glm::vec3 C1 = p - m_p1;
	glm::vec3 C2 = p - m_p2;
	glm::vec3 C3 = p - m_p3;

	if (glm::dot(m_Normal,glm::cross(edge1,C1)) > 1e-6)
		return std::numeric_limits<float>::max();

	if (glm::dot(m_Normal, glm::cross(edge2, C2)) > 1e-6)
		return std::numeric_limits<float>::max();

	if (glm::dot(m_Normal, glm::cross(edge3, C3)) > 1e-6)
		return std::numeric_limits<float>::max();

	return t;
}