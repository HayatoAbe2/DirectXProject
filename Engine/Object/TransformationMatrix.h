#pragma once
#include "../Math/MathUtils.h"

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};