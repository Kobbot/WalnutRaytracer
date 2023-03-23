#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Material {
	glm::vec3 Albedo{ 1.0f };
	float Roughness = 1.0f;
	float Metallic = 0.0f;
};