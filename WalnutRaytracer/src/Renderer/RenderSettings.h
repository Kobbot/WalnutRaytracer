#pragma once

struct RenderSettings
{
	bool Accumulate = true;
	bool GammaCorrection = false;
	bool SoftShadows = true;
	int ShadowSamples = 1;
	bool LogAccumulation = false;
	bool AllowTransparency = true;
};