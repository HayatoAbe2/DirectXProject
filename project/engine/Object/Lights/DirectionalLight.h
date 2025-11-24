#pragma once
#include "MathUtils.h"

struct DirectionalLight {
	Vector4 color = {1.0f,1.0f,1.0f,1.0f};
	Vector3 direction = Normalize({ 0.2f, -0.6f, 1.5f });; // 向き(単位ベクトル)
	float intensity = 1.0f; // 輝度
};