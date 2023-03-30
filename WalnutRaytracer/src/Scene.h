#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Primitives/Sphere.h"
#include "Primitives/Plane.h"
#include "Primitives/Triangle.h"

#include "Lightsources/PointLight.h"
#include "Lightsources/GlobalLight.h"
#include "Lightsources/SphereLight.h"


struct Scene
{
	//Primitives
	std::vector<Sphere> Spheres;
	std::vector<Plane> Planes;
	std::vector<Triangle> Triangles;

	//Material list
	std::vector<Material> Materials;

	//Light sources
	GlobalLight GlobalLight;
	std::vector<PointLight> PointLights;
	std::vector<SphereLight> SphereLights;
};