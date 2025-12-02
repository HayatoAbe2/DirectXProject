#pragma once
#include "Entity.h"
#include "WeaponStatus.h"
#include "RangedWeapon.h"
#include <MathUtils.h>
#include <vector>
#include <memory>

class GameContext;
class MapCheck;
class Camera;
class Player;
class BulletManager;

class Enemy {
public:
	Enemy(std::unique_ptr<Entity> model,Vector3 pos,std::unique_ptr<RangedWeapon> rWeapon);


	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="context">コンテキスト</param>
	void Update(GameContext* context, MapCheck* mapCheck, Player* player, BulletManager* bulletManager);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="context">コンテキスト</param>
	/// <param name="camera">カメラ</param>
	void Draw(GameContext* context, Camera* camera);

	void Hit(int damage,Vector3 from);
	void Fall();

	Transform GetTransform() const { return model_->GetTransform(); }
	float GetRadius() const { return radius_; }
	bool IsDead() { return isDead_; }
	bool IsStunning() { return stunTimer_ > 0; }

private:
	// 半径
	float radius_ = 0.5f;

	// 移動の速さ
	float moveSpeed_ = 0.1f;

	// 移動
	Vector3 velocity_{};

	// 攻撃の向き
	Vector3 attackDirection_ = {};

	// モデル
	std::unique_ptr<Entity> model_ = nullptr;

	// 武器
	std::unique_ptr<RangedWeapon> rangedWeapon_ = nullptr;

	// 射撃クールダウン
	int attackCoolTimer_ = 90;

	// hp
	int hp_ = 10;

	// 弾速
	float bulletSpeed_ = 0.1f;

	// プレイヤー
	Player* target_ = nullptr;

	// 検知範囲
	float searchRadius_ = 8.0f;
	float defaultSearchRadius_ = 8.0f;

	// 見失い距離
	float loseSightRadius_ = 15.0f;

	// 見失いカウント
	int loseSightTime_ = 120;
	int loseSightTimer_ = 0;

	// 死亡フラグ
	bool isDead_ = false;

	// スタン時間
	int stunTimer_ = 0;

	// 落下
	bool isFall_ = false;

	// 行動変更タイマー
	int actionChangeTimer_ = 0;
	const int actionChangeInterval_ = 20;

	// 攻撃モーション
	int attackMotionStart_ = 30;
	float EaseIn(float start, float end, float t);
};

