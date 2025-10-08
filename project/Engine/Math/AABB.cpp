#include "AABB.h"

bool AABB::CheckCollision(const AABB& aabb1, const AABB& aabb2) {
	if ((aabb1.min_.x <= aabb2.max_.x && aabb1.max_.x >= aabb2.min_.x) && // x軸
	    (aabb1.min_.y <= aabb2.max_.y && aabb1.max_.y >= aabb2.min_.y) && // y軸
	    (aabb1.min_.z <= aabb2.max_.z && aabb1.max_.z >= aabb2.min_.z)) { // z軸
		return true;
	}
	return false;
}
