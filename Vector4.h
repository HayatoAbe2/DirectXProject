#pragma once
#include "Matrix4x4.h"

struct Vector4 {
	float x;
	float y;
	float z;
	float w;
};

Vector4 HSVtoRGB(float h, float s = 1.0f, float v = 1.0f);

Vector4 operator*(const Matrix4x4& mat, const Vector4& v);
