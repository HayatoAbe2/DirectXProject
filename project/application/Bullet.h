#pragma once
#include "MathUtils.h"
#include "WeaponStatus.h"
#include "Entity.h"
#include <memory>

class Camera;
class GameContext;

class Bullet {
public:
	Bullet(std::unique_ptr<Entity> model,const Vector3& velocity,const RangedWeaponStatus& status);
	void Update();
	void Draw(GameContext* context,Camera* camera);
	bool IsEnemyBullet() { return isEnemyBullet_; }
	Transform GetTransform() { return model_->GetTransform(); }
	int GetDamage() { return status_.damage; }
	
private:
	// 速度
	Vector3 velocity_{};

	// モデル
	std::unique_ptr<Entity> model_ = nullptr;

	// 弾ステータス
	RangedWeaponStatus status_;

	// 敵の弾フラグ
	bool isEnemyBullet_ = false;
};

