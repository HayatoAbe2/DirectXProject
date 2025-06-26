#pragma once
#include <array>
#include <numbers>
#include "Math.h"
#include "Model.h"
#include "Transform.h"
class DeathParticles {
public:

	void Initialize(Model* model, const Vector3& position);
	void Update();
	void Draw();

	const Transform& GetTransform() { return worldTransform_; };

private:
	Transform worldTransform_; // ワールドトランスフォーム
	Model* model_ = nullptr;        // モデル
	
	// パーティクルの個数
	static inline const uint32_t kNumParticles = 8;
	// 消えるまでの時間
	static inline const float kDuration = 1.0f;
	// 移動の速さ
	static inline const float kSpeed = 0.075f;
	// 分割した1個分の角度
	static inline const float kAngleUnit = 2.0f * float(std::numbers::pi) / float(kNumParticles);

	// 終了フラグ
	bool isFinished_ = false;
	// 経過時間
	float counter_ = 0.0f;

	// 色
	Vector4 color_;

	std::array<Transform, kNumParticles> worldTransforms_;
};
