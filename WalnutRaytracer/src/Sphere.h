#pragma once

#include "Primitive.h"

class Sphere : public Primitive 
{
public:
	Sphere(glm::vec3 position, glm::vec3 albedo, float radius);
	float GetRadius() const { return m_Radius; }
	void Intersect() { };
private:
	float m_Radius	{0.5f};
};