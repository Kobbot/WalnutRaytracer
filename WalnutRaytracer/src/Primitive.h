#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Ray.h"
#include "Material.h"

class Primitive
{
public:
	Primitive(glm::vec3 position, Material mat): m_Position(position), m_Mat(mat) { }
	virtual float Intersect(const Ray& ray) const =0;
	glm::vec3 GetPosition() const { return m_Position; };
	void SetPosition(glm::vec3 position) { m_Position = position; };
	Material GetMaterial() const { return m_Mat; }
protected:
	glm::vec3 m_Position {0.0f};
	Material m_Mat;
};