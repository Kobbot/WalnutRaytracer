#pragma once

#include "Primitive.h"

class Sphere : public Primitive 
{
public:
	Sphere(glm::vec3 position, int matIndex, float radius);
	float GetRadius() const { return m_Radius; }
	//Primitive virtuals
	float Intersect(const Ray& ray) const;
private:
	float m_Radius	{0.5f};
};