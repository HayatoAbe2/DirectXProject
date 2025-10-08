#include "Vector4.h"
#include "cmath"

Vector4 HSVtoRGB(float h, float s, float v) {
	float c = v * s;
	float x = c * float(1 - std::fabs(std::fmod(h * 6, 2) - 1));
	float m = v - c;
	float r, g, b;

	if (h < 1.0f / 6) {
		r = c; g = x; b = 0;
	} else if (h < 2.0f / 6) {
		r = x; g = c; b = 0;
	} else if (h < 3.0f / 6) {
		r = 0; g = c; b = x;
	} else if (h < 4.0f / 6) {
		r = 0; g = x; b = c;
	} else if (h < 5.0f / 6) {
		r = x; g = 0; b = c;
	} else { r = c; g = 0; b = x; }

	return Vector4(r + m, g + m, b + m, 1.0f);
}

Vector4 operator*(const Matrix4x4& mat, const Vector4& v) {
	Vector4 result;
	result.x = mat.m[0][0] * v.x + mat.m[0][1] * v.y + mat.m[0][2] * v.z + mat.m[0][3] * v.w;
	result.y = mat.m[1][0] * v.x + mat.m[1][1] * v.y + mat.m[1][2] * v.z + mat.m[1][3] * v.w;
	result.z = mat.m[2][0] * v.x + mat.m[2][1] * v.y + mat.m[2][2] * v.z + mat.m[2][3] * v.w;
	result.w = mat.m[3][0] * v.x + mat.m[3][1] * v.y + mat.m[3][2] * v.z + mat.m[3][3] * v.w;
	return result;
}
