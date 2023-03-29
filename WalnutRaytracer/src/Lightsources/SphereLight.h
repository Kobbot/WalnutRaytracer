#pragma once

#include "../Primitives/Sphere.h"

struct SphereLight {
	float Intensity{ 10.0f };
	Sphere Sphere{ { 0.0f, 0.0f, 0.0f }, 1, 0.4f };
};

