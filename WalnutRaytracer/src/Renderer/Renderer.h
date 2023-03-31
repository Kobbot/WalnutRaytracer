#pragma once

#include "Walnut/Image.h"

#include <memory>
#include <glm/glm.hpp>

#include "../Camera.h"
#include "../Ray.h"
#include "../Scene.h"
#include "../Primitives/Sphere.h"
#include "../Primitives/Primitive.h"
#include "../Primitives/Triangle.h"

#include "RenderSettings.h"
#include "GeometryTracer.h"
#include "ShadowTracer.h"
#include "HitPayload.h"

namespace WN = Walnut;

class Renderer
{
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render(const Scene& scene, const Camera& camera);

	std::shared_ptr<WN::Image> GetFinalImage() const { return m_FinalImage; }

	void ResetFrameIndex() { m_FrameIndex = 1; }
	RenderSettings& GetSettings() { return m_Settings; }
private:
	glm::vec4 PerPixel(uint32_t index); //RayGen Shader. Invoked per pixel rendered. Might cast multiple rays per pixel.
	glm::vec3 Renderer::ColorRay(uint32_t index);

private:
	std::shared_ptr<WN::Image> m_FinalImage;
	RenderSettings m_Settings;

	GeometryTracer m_GeometryTracer = GeometryTracer(&m_Settings);
	ShadowTracer m_ShadowTracer = ShadowTracer(&m_Settings);

	std::vector<uint32_t> m_ImageHorizontalIter, m_ImageVerticalIter;

	const Scene* m_ActiveScene = nullptr;
	const Camera* m_ActiveCamera = nullptr;

	uint32_t* m_ImageData = nullptr;
	glm::vec4* m_AccumulationData = nullptr;

	uint32_t m_FrameIndex = 1;
};