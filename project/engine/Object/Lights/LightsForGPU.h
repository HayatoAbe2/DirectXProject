#pragma once
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

// ポイントライト最大数(PSと合わせる)
const int maxPointLights = 32;
const int maxSpotLights = 16;

struct LightsForGPU {
	DirectionalLight directionalLight;
	PointLight pointLights[32];
	SpotLight spotLights[16];
};