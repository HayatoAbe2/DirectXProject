#pragma once
#include "Math/MathUtils.h"

struct PointLight {
	Vector4 color = {1.0f,1.0f,1.0f,1.0f};
	Vector3 position = {};
	float intensity = 1.0f;
	float radius = 3.0f;
	float decay = 0.5f;
	float padding[2];
};