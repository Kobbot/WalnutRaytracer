#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "../Ray.h"
#include "../Materials/Material.h"

class Primitive
{
public:
	Primitive(glm::vec3 position, int matIndex): m_Position(position), m_MaterialIndex(matIndex) { }
	virtual float Intersect(const Ray& ray) const =0;
	glm::vec3 GetPosition() const { return m_Position; };
	void SetPosition(glm::vec3 position) { m_Position = position; };
	int GetMaterialIndex() const { return m_MaterialIndex; }
protected:
	glm::vec3 m_Position {0.0f};
	int m_MaterialIndex;
};