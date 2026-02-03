#pragma once
#include "Transform.h"
#include "MathUtils.h"
#include "Weapon.h"
#include "GameContext.h"
#include <vector>
#include <memory>

class Camera;
class MapCheck;
class Weapon;
class ItemManager;
class BulletManager;

class Player {
public:
	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="playerModel">モデル</param>
	void Initialize(std::unique_ptr<Model> playerModel, std::unique_ptr<Model> playerShadow, GameContext* context);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="context">コンテキスト</param>
	void Update(MapCheck* mapCheck, ItemManager* itemManager_, Camera* camera, BulletManager* bulletManager);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="context">コンテキスト</param>
	/// <param name="camera">カメラ</param>
	void Draw(Camera* camera);

	// 被弾
	void Hit(float damage, Vector3 from);

	void Move(MapCheck* mapCheck);
	void Shoot(BulletManager* bulletManager, Camera* camera);
	void Boost(MapCheck* mapCheck);
	void Fall();

	Transform GetTransform() const { return transform_; }
	float GetRadius() const { return radius_; }
	float GetInteractRadius() const { return interactRadius_; }
	Weapon* GetWeapon() { return weapon_.get(); }
	Weapon* GetSubWeapon() { return subWeapon_.get(); }
	float GetHP() { return hp_; }
	float GetMaxHP() { return maxHp_; }
	bool IsBoosting() { return isUsingBoost_; }

	void SetTransform(const Transform& transform) { transform_ = transform; }
	std::unique_ptr<Weapon> DropWeapon() { if (weapon_ && subWeapon_) { return std::move(weapon_); } else { return nullptr; } };
	void SetWeapon(std::unique_ptr<Weapon> weapon);
	bool IsDead() { return hp_ <= 0; }

	void Stop() { boostTime_ = 0; }

private:
	GameContext* context_ = nullptr;

	// 照準
	std::unique_ptr<Model> direction_ = nullptr;

	// トランスフォーム
	Transform transform_;
	Transform weaponTransform_;

	// 半径
	float radius_ = 0.5f;

	// アイテム取得範囲
	float interactRadius_ = 1.5f;

	// 速度
	Vector3 velocity_ = { 0,0,0 };

	// 移動の速さ
	float moveSpeed_ = 0.2f;
	const float defaultMoveSpeed_ = 0.2f;

	// 攻撃の向き
	Vector3 attackDirection_ = {};

	// hp
	float hp_ = 40;
	float maxHp_ = 40;

	// スタン時間
	int stunTimer_ = 0;
	bool isFall_ = false;
	Vector3 landPos_{};
	int redTime_ = 0;
	int invincibleTimer_ = 0;
	int invincibleTime_ = 20;

	// モデル
	std::unique_ptr<Model> model_ = nullptr;
	std::unique_ptr<Model> shadowModel_ = nullptr;
	std::unique_ptr<InstancedModel> instancing_ = nullptr;
	Transform instancingTransforms[4]{};

	// ダッシュ
	bool canBoost_ = false;
	bool isUsingBoost_ = false;
	int boostTime_ = 0;
	int maxBoostTime_ = 10;
	float boostSpeed_ = 0.3f;
	Vector3 boostDir_{};
	int boostCoolTime_ = 10;

	// 射撃
	int shootCoolTime_ = 0;
	int extraBulletWaitTime_ = 0;
	bool canShootExtraBullet_ = false;

	// 所持武器
	std::unique_ptr<Weapon> weapon_ = nullptr;
	std::unique_ptr<Weapon> subWeapon_ = nullptr;

	// 移動パーティクル
	std::unique_ptr<ParticleSystem> moveParticle_;
	const int moveParticleNum_ = 20;
	float moveParticleRange_ = 1.0f;
	int moveParticleEmitTimer_ = 0;
	int moveParticleEmitInterval_ = 20;

	float dirDisplayAlpha_ = 0.3f;
};

