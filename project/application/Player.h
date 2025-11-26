#pragma once
#include "Transform.h"
#include "MathUtils.h"
#include "RangedWeapon.h"
#include <vector>
#include <memory>

class Entity;
class GameContext;
class Camera;
class MapCheck;
class RangedWeapon;
class ItemManager;
class BulletManager;

class Player {
public:
	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="playerModel">モデル</param>
	void Initialize(Entity* playerModel, GameContext* context);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="context">コンテキスト</param>
	void Update(GameContext* context, MapCheck* mapCheck, ItemManager* itemManager_, Camera* camera, BulletManager* bulletManager);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="context">コンテキスト</param>
	/// <param name="camera">カメラ</param>
	void Draw(GameContext* context, Camera* camera);

	// 被弾
	void Hit(int damage, Vector3 from);

	Transform GetTransform() const { return transform_; }
	float GetRadius() const { return radius_; }
	float GetInteractRadius() const { return interactRadius_; }
	void SetTransform(const Transform& transform) { transform_ = transform; }
	std::unique_ptr<RangedWeapon> DropRangedWeapon() { return std::move(rangedWeapon_); };
	void SetWeapon(std::unique_ptr<RangedWeapon> rangedWeapon);
	bool IsDead() { return hp_ <= 0; }


private:
	// 操作説明
	std::unique_ptr<Entity> control_ = nullptr;
	std::unique_ptr<Entity> life_ = nullptr;

	// トランスフォーム
	Transform transform_;

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
	int hp_ = 30;
	int maxHp_ = 30;

	// スタン時間
	int stunTimer_ = 0;

	// モデル
	Entity* model_ = nullptr;

	// 射撃
	int shootCoolTime_ = 0;

	// 所持武器
	std::unique_ptr<RangedWeapon> rangedWeapon_ = nullptr;
};

