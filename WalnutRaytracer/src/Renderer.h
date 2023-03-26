#pragma once

#include "Walnut/Image.h"

#include <memory>
#include <glm/glm.hpp>

#include "Camera.h"
#include "Ray.h"
#include "Scene.h"
#include "Primitives/Sphere.h"
#include "Primitives/Primitive.h"

namespace WN = Walnut;

class Renderer
{
public:
	struct Settings
	{
		bool Accumulate = true;
		bool GammaCorrection = false;
	};
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render(const Scene& scene, const Camera& camera);

	std::shared_ptr<WN::Image> GetFinalImage() const { return m_FinalImage; }

	void ResetFrameIndex() { m_FrameIndex = 1; }
	Settings& GetSettings() { return m_Settings; }
private:
	struct HitPayload
	{
		float HitDistance;
		glm::vec3 WorldPosition;
		glm::vec3 WorldNormal;

		int ObjectIndex;
		int ObjectType; //1. Sphere 2. Plane
		int MaterialIndex;
	};

	glm::vec4 PerPixel(uint32_t index); //RayGen Shader. Invoked per pixel rendered. Might cast multiple rays per pixel.
	HitPayload TraceRay(const Ray& ray);
	HitPayload ClosestHit(const Ray& ray, float hitDistance, int objectIndex, int objectType);
	HitPayload MissShader(const Ray& ray);
	float LightContribution(const Ray& surfaceNormal);

private:
	std::shared_ptr<WN::Image> m_FinalImage;
	Settings m_Settings;

	std::vector<uint32_t> m_ImageHorizontalIter, m_ImageVerticalIter;

	const Scene* m_ActiveScene = nullptr;
	const Camera* m_ActiveCamera = nullptr;

	uint32_t* m_ImageData = nullptr;
	glm::vec4* m_AccumulationData = nullptr;

	uint32_t m_FrameIndex = 1;
};