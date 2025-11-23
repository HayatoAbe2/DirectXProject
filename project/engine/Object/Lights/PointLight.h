#pragma once
#include "MathUtils.h"

struct PointLight {
	Vector4 color = {1.0f,1.0f,1.0f,1.0f};
	Vector3 position = {};
	float intensity = 1.0f;
};