#pragma once
#include "Math/MathUtils.h"
#include <cstdint>
#include <d3d12.h>

struct MaterialData {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
	float shininess = 32;
	UINT useTexture;
	float padding2[2];
};