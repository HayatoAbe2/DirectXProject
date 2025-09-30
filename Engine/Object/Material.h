#pragma once
#include "../Math/Vector4.h"
#include "../Math/Matrix4x4.h"
#include <cstdint>
#include <d3d12.h>

struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
	UINT useTexture;
};