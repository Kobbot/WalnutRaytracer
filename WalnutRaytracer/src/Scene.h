#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Primitive.h"
#include "Sphere.h"

struct Scene
{
	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;	
};