#pragma once

#include <glm/glm.hpp>

#include "../Ray.h"

struct HitPayload
{
	Ray ray;

	float HitDistance;
	glm::vec3 WorldPosition;
	glm::vec3 WorldNormal;

	int ObjectIndex;
	int ObjectType; //1. Sphere 2. Plane
	int MaterialIndex;
};