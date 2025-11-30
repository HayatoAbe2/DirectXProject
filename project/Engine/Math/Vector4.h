#pragma once
#include "Matrix4x4.h"

struct Vector4 {
	float x;
	float y;
	float z;
	float w;

	// 演算子オーバーロード
	Vector4& operator*=(float s) {
		x *= s;
		y *= s;
		z *= s;
		w *= s;
		return *this;
	}

	Vector4& operator-=(const Vector4& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}

	Vector4& operator+=(const Vector4& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	Vector4& operator/=(float s) {
		if (s != 0.0f) {
			x /= s;
			y /= s;
			z /= s;
			w /= s;
		}
		return *this;
	}
};

Vector4 HSVtoRGB(float h, float s = 1.0f, float v = 1.0f);

Vector4 operator*(const Matrix4x4& mat, const Vector4& v);
