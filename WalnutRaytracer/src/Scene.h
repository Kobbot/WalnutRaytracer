#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Primitives/Primitive.h"
#include "Primitives/Sphere.h"
#include "Primitives/Plane.h"

struct Scene
{
	std::vector<Sphere> Spheres;
	std::vector<Plane> Planes;

	std::vector<Material> Materials;
};