#pragma once
#include "ModelData.h"
#include "Math.h"
#include <vector>
#include "AABB.h"

class Player;
/// <summary>
/// 敵キャラ
/// </summary>
class Enemy {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ModelData* model, const Vector3& position);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void OnCollision(const Player* player);

	AABB GetAABB() { return aabb_; };
	const Transform& GetTransform() { return worldTransform_; }

private:
	Transform worldTransform_; // ワールドトランスフォーム
	ModelData* model_ = nullptr;        // モデル

	static inline const float kWalkSpeed = 0.01f; // 歩行速度
	Vector3 velocity_ = {}; // 速度

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kBlank = kHeight / 2.0f;

	// 最初の角度(度)
	static inline const float kWalkMotionAngleStart = -30.0f;
	// 前後の角度(度)
	static inline const float kWalkMotionAngleEnd = 60.0f;
	// アニメーションの周期(秒)
	static inline const float kWalkMotionTime = 1.5f;
	// 経過時間
	float walkTimer_ = 0.0f;

	AABB aabb_;
};
