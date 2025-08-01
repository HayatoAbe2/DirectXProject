#pragma once

struct Vector4 {
	float x;
	float y;
	float z;
	float w;
};

Vector4 HSVtoRGB(float h, float s = 1.0f, float v = 1.0f);