#pragma once

#include <glm/glm.hpp>
#include <vector>

class Primitive
{
public:
	Primitive(glm::vec3 position, glm::vec3 albedo): m_Position(position), m_Albedo(albedo) { }
	virtual void Intersect() { };
	glm::vec3 GetPosition() const { return m_Position; };
	void SetPosition(glm::vec3 position) { m_Position = position; };
	glm::vec3 GetAlbedo() const { return m_Albedo; }
private:
	glm::vec3 m_Position {0.0f};
	glm::vec3 m_Albedo{ 1.0f };
};