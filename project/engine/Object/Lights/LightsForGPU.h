#pragma once
#include "DirectionalLight.h"
#include "PointLight.h"

// ポイントライト最大数(PSと合わせる)
const int maxPointLights = 32;

struct LightsForGPU {
	DirectionalLight directionalLight;
	PointLight pointLights[32];
	uint32_t pointLightCount;
	float padding[3];
};