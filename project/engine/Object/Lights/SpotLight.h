#pragma once
#include "MathUtils.h"

struct SpotLight {
	Vector4 color = {1.0f,1.0f,1.0f,1.0f};
	Vector3 position = {};
	float intensity = 1.0f;
	Vector3 direction = {1.0f,0,0};
	float distance = 10.0f;
	float decay = 0.5f;
	float cosAngle = 0.8f;
	float cosFalloffStart = 1.0f;
	float padding;
};