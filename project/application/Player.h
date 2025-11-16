#pragma once
#include "Transform.h"
#include "MathUtils.h"
#include "Bullet.h"
#include "RangedWeapon.h"
#include <vector>
#include <memory>

class Entity;
class GameContext;
class Camera;
class MapCheck;
class RangedWeapon;
class ItemManager;

class Player {
public:
	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="playerModel">モデル</param>
	void Initialize(Entity* playerModel);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="context">コンテキスト</param>
	void Update(GameContext* context,MapCheck* mapCheck,ItemManager* itemManager, Camera* camera, std::vector<std::unique_ptr<Bullet>>& bullets);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="context">コンテキスト</param>
	/// <param name="camera">カメラ</param>
	void Draw(GameContext* context, Camera* camera);

	Transform GetTransform() const { return transform_; }
	float GetRadius() const { return radius_; }

	void SetTransform(const Transform& transform) { transform_ = transform; }

	void SetWeapon(std::unique_ptr<RangedWeapon> rangedWeapon);

	// 被弾
	void Hit(int damage) { hp_ -= damage; if (hp_ <= 0) {} }

private:
	// トランスフォーム
	Transform transform_;

	// 半径
	float radius_ = 0.5f;
	
	// 速度
	Vector3 velocity_ = { 0,0,0 };

	// 移動の速さ
	float moveSpeed_ = 0.2f;

	// 攻撃の向き
	Vector3 attackDirection_ = {};

	// hp
	int hp_ = 10;

	// モデル
	Entity* model_ = nullptr;

	// 射撃
	int shootCoolTime_ = 0;

	// 所持武器
	std::unique_ptr<RangedWeapon> rangedWeapon_ = nullptr;
};

