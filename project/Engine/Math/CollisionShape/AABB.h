#pragma once
#include "Vector3.h"

struct AABB {	
	Vector3 min; // 最小点
	Vector3 max; // 最大点
};

bool IsCollision(const AABB& aabb1, const AABB& aabb2);
bool IsCollision(const AABB& aabb, const Vector3& point);