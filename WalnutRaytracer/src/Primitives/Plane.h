#pragma once

#include "Primitive.h"

class Plane : public Primitive
{
public:
	Plane(glm::vec3 position, int matIndex, glm::vec3 normal) : Primitive(position, matIndex), m_Normal(normal) {};
	glm::vec3 GetNormal() const { return m_Normal; }
	//Primitive virtuals
	float Intersect(const Ray& ray) const;
private:
	glm::vec3 m_Normal{ 0.0f, 0.0f, 1.0f };
};