#pragma once
#include <vector>
#include "AABB.h"
#include "Math.h"
#include "Model.h"
#include "Input.h"

class MapChipField;
class Enemy;
class Camera;
class Graphics;
/// <summary>
/// 自キャラ
/// </summary>
class Player {
	// マップとの当たり判定情報
	struct CollisionMapInfo {
		bool isHitCeiling = false;
		bool isLanding = false;
		bool isHitWall = false;
		Vector3 velocity;
	};

	// 角
	enum Corner {
		kRightBottom,
		kLeftBottom,
		kRightTop,
		kLeftTop,
		kNumCorner // 角の数 = 4
	};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, const Vector3& position, Input* input);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	// 入力の受付
	/// </summary>
	void InputMove();

	/// <summary>
	/// マップとの衝突判定
	/// </summary>
	/// <param name="info">衝突判定関連の情報</param>
	void CheckMapCollision(CollisionMapInfo& info);

	/// <summary>
	/// 中心座標から指定した角の座標を取得する
	/// </summary>
	/// <param name="center">中心座標</param>
	/// <param name="corner">どの角か</param>
	/// <returns></returns>
	Vector3 CornerPosition(const Vector3& center, Corner corner);
	
	/// <summary>
	/// 天井との衝突判定
	/// </summary>
	/// <param name="info">衝突判定関連の情報</param>
	void CheckHitTop(CollisionMapInfo& info);
	/// <summary>
	/// 地面との衝突判定
	/// </summary>
	/// <param name="info">衝突判定関連の情報</param>
	void CheckHitBottom(CollisionMapInfo& info);
	/// <summary>
	/// 左の壁との衝突判定
	/// </summary>
	/// <param name="info">衝突判定関連の情報</param>
	void CheckHitLeft(CollisionMapInfo& info);
	/// <summary>
	/// 右の壁との衝突判定
	/// </summary>
	/// <param name="info">衝突判定関連の情報</param>
	void CheckHitRight(CollisionMapInfo& info);

	/// <summary>
	/// 接地状態の切り替え
	/// </summary>
	/// <param name="info">衝突判定関連の情報</param>
	void SwitchLanding(CollisionMapInfo& info);

	/// <summary>
	/// 座標を移動
	/// </summary>
	/// <param name="info">衝突判定関連の情報</param>
	void Move(const CollisionMapInfo& info);

	/// <summary>
	/// 天井に衝突した時の処理
	/// </summary>
	/// <param name="info">衝突判定関連の情報</param>
	void OnHitTop(const CollisionMapInfo& info);
	/// <summary>
	/// 地面に接触している場合の処理
	/// </summary>
	/// <param name="info">衝突判定関連の情報</param>
	void OnHitBottom(const CollisionMapInfo& info);
	/// <summary>
	/// 壁に接触している場合の処理
	/// </summary>
	/// <param name="info">衝突判定関連の情報</param>
	void OnHitWall(const CollisionMapInfo& info);

	/// <summary>
	/// 敵との衝突時
	/// </summary>
	/// <param name="enemy">敵</param>
	void OnCollision(const Enemy* enemy);

	void Draw(Camera& camera, Graphics& graphics);

	/// <summary>
	/// ワールドトランスフォームを取得
	/// </summary>
	/// <returns>プレイヤーのワールドトランスフォーム</returns>
	const Transform& GetWorldTransform() const { return worldTransform_; };
	
	/// <summary>
	/// 速度取得
	/// </summary>
	/// <returns>プレイヤー速度</returns>
	const Vector3& GetVelocity() const { return velocity_; };
	
	/// <summary>
	/// マップチップ設定
	/// </summary>
	/// <param name="mapChipField"></param>
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; };

	AABB GetAABB() { return aabb_; };
	const Transform& GetTransform() { return worldTransform_; }

	bool IsDead() const { return isDead_; }
	void SetIsDead(bool isDead) { isDead_ = isDead; }

private:
	// ワールドトランスフォーム
	Transform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	
	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	Vector3 velocity_ = {};
	static inline const float kAcceleration = 0.01f;
	static inline const float kAttenuation = 0.1f;
	static inline const float kLimitRunSpeed = 0.2f;

	// 左右
	enum class LRDirection { kRight, kLeft };
	// 向き
	LRDirection lrDirection_ = LRDirection::kRight;
	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
	// 旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;

	// 設置状態フラグ
	bool onGround_ = true;
	// 重力加速度(下方向)
	static inline const float kGravityAcceleration = 0.04f;
	// 最大落下速度
	static inline const float kLimitFallSpeed = 1.0f;
	// ジャンプ初速
	static inline const float kJumpAcceleration = 0.5f;
	
	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	static inline const float kBlank = kHeight / 2.0f;

	static inline const float kAttenuationLanding = 0.02f;
	static inline const float kAttenuationWall = 0.2f;
	static inline const float kCollisionOffset = 0.05f;

	AABB aabb_;

	bool isDead_ = false;

	Input* input_;
};
