#pragma once

#include "RenderSettings.h"
#include "HitPayload.h"

#include "../Camera.h"
#include "../Ray.h"
#include "../Scene.h"


class GeometryTracer
{
public:
	GeometryTracer(RenderSettings* settings) : m_Settings(settings) {};
	HitPayload TraceRay(const Ray& ray, const Scene* scene);
	HitPayload ClosestHit(const Ray& ray, const Scene* scene, float hitDistance, int objectIndex, int objectType);
	HitPayload MissShader(const Ray& ray);
private:
	RenderSettings* m_Settings = nullptr;
};