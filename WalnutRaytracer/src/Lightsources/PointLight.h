#pragma once
#include <glm/glm.hpp>

#include "../Scene.h"

class PointLight
{
public:
	PointLight(glm::vec3 position, float intensity) : m_Position(position), m_Intensity(intensity) { }
	virtual float VisibleFrom(const Scene& scene, glm::vec3 position);
protected:
	glm::vec3 m_Position{ 0.0f };
	float m_Intensity{ 1.0f };
};