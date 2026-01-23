#pragma once
#include "MathUtils.h"
#include <string>
#include <vector>
#include <memory>

struct Node {
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<std::unique_ptr<Node>> children;
	std::vector<int> meshIndices;
	uint32_t transformCBHandle_ = 0; // トランスフォームCBハンドル

    Node() = default;
    ~Node() = default;

    // コピー禁止
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    // ムーブ許可
    Node(Node&&) = default;
    Node& operator=(Node&&) = default;
};