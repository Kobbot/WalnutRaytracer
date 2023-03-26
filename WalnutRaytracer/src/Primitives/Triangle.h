#pragma once

#include "Primitive.h"

class Triangle : public Primitive
{
public:
	Triangle(glm::vec3 position, int matIndex, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 normal) : Primitive(position, matIndex),
		m_p1(p1), m_p2(p2), m_p3(p3), m_Normal(normal) {};
	float Intersect(const Ray& ray) const;
private:
	glm::vec3 m_p1;
	glm::vec3 m_p2;
	glm::vec3 m_p3;
	glm::vec3 m_Normal;
};