#pragma once
#include "MathUtils.h"
#include <string>
#include <vector>

struct Node {
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node> children;
};