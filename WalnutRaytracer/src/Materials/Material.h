#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Material {
	glm::vec3 Albedo{ 1.0f };
	float Roughness = 1.0f;
	float Metallic = 0.0f;
	//What fraction of light gets reflected.
	float Reflectivity = 0.4f;
	// Material needs to at have Transparency > 0 to be dielectric.
	float Transparency = 0.0f;
	float Refractivity = 0.0f;
};