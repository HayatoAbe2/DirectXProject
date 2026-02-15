#pragma once
#include "Object/Lights/DirectionalLight.h"
#include "Object/Lights/PointLight.h"
#include "Object/Lights/SpotLight.h"

// ポイントライト最大数(PSと合わせる)
const int maxPointLights = 32;
const int maxSpotLights = 16;

struct LightsForGPU {
	DirectionalLight directionalLight;
	PointLight pointLights[32];
	SpotLight spotLights[16];
};