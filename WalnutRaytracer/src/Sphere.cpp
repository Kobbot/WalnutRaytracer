#include "Sphere.h"

Sphere::Sphere(glm::vec3 position, glm::vec3 albedo, float radius)
	: Primitive(position, albedo), m_Radius(radius)
{}

