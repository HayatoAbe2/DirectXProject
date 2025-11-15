#pragma once
#include "MathUtils.h"
#include "WeaponStatus.h"
#include <memory>

class Entity;
class Camera;
class GameContext;

class Bullet {
public:
	Bullet(std::unique_ptr<Entity> model,const Vector3& velocity,const RangedWeaponStatus& status);
	void Update();
	void Draw(GameContext* context,Camera* camera);
private:
	// 速度
	Vector3 velocity_{};

	// モデル
	std::unique_ptr<Entity> model_ = nullptr;

	// 弾ステータス
	RangedWeaponStatus status_;
};

