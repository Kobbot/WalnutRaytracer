#pragma once
#include <glm/glm.hpp>

struct PointLight {
	float Intensity{ 1.0f };
	glm::vec3 Position{ 0.0f };
};