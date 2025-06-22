#pragma once
#include "Math.h"

class AABB {
public:
	bool CheckCollision(const AABB& aabb1, const AABB& aabb2);
	
	Vector3 min_; // 最小点
	Vector3 max_; // 最大点
};