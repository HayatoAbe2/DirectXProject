#pragma once
#include "Math/MathUtils.h"
#include <string>
#include <vector>
#include <memory>

struct ModelNode {
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<std::unique_ptr<ModelNode>> children;
	std::vector<int> meshIndices;
	uint32_t transformCBHandle_ = 0; // トランスフォームCBハンドル

    ModelNode() = default;
    ~ModelNode() = default;

    // コピー禁止
    ModelNode(const ModelNode&) = delete;
    ModelNode& operator=(const ModelNode&) = delete;

    // ムーブ許可
    ModelNode(ModelNode&&) = default;
    ModelNode& operator=(ModelNode&&) = default;
};