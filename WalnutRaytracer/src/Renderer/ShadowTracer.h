#pragma once

#include "RenderSettings.h"
#include "HitPayload.h"
#include "GeometryTracer.h"

#include "../Camera.h"
#include "../Ray.h"
#include "../Scene.h"

#include "Walnut/Random.h"
namespace WN = Walnut;

class ShadowTracer
{
public:
	ShadowTracer(RenderSettings* settings) : m_Settings(settings) {};
	float LightContribution(const Ray& surfaceNormal, const HitPayload& payload, const Scene* scene, GeometryTracer* tracer);
private:
	RenderSettings* m_Settings = nullptr;
};