#pragma once

#include <glm/glm.hpp>

struct GlobalLight
{
	float Intensity{ 1.0f };
	glm::vec3 Direction { glm::normalize(glm::vec3(-1, -1, -1)) };
};
