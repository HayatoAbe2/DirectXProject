#pragma once
#include "MathUtils.h"
#include <string>
#include <vector>

struct Node {
	Matrix4x4 localMatrix;
	Matrix4x4 worldMatrix;
	std::string name;
	std::vector<Node> children;
	std::vector<int> meshIndices;
};