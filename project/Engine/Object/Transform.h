#pragma once
#include "../Math/Vector3.h"
struct Transform {
	Vector3 scale = {1.0f,1.0f,1.0f};
	Vector3 rotate = {};
	Vector3 translate = {};
};