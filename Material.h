#pragma once
#include "Vector4.h"
#include "Matrix4x4.h"
#include <cstdint>
#include <d3d12.h>

struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
	UINT useTexture;
};