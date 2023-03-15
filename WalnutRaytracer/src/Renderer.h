#pragma once

#include "Walnut/Image.h"

#include <memory>
#include <glm/glm.hpp>

#include "Camera.h"
#include "Ray.h"
#include "Scene.h"
#include "Sphere.h"


namespace WN = Walnut;

class Renderer
{
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render(const Scene& scene, const Camera& camera);

	std::shared_ptr<WN::Image> GetFinalImage() const { return m_FinalImage; }


private:
	glm::vec4 TraceRay(const Scene& scene, const Ray& ray);
private:
	std::shared_ptr<WN::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;
};