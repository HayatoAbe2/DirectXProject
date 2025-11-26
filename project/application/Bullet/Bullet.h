#pragma once
#include "MathUtils.h"
#include "WeaponStatus.h"
#include "Entity.h"
#include <memory>

class Camera;
class GameContext;
class MapCheck;

class Bullet {
public:
	Bullet(std::unique_ptr<Entity> model,const Vector3& direction,const RangedWeaponStatus& status,bool isEnemyBullet);
	virtual void Update(MapCheck* mapCheck);
	virtual void Draw(GameContext* context,Camera* camera);
	virtual void Hit();

	bool IsEnemyBullet() { return isEnemyBullet_; }
	Transform GetTransform() { return model_->GetTransform(); }
	int GetDamage() { return status_.damage; }
	bool IsDead() { return isDead_; }
	
protected:
	// 速度
	Vector3 velocity_{};

	// モデル
	std::unique_ptr<Entity> model_ = nullptr;

	// 弾ステータス
	RangedWeaponStatus status_;

	// 生存時間
	int lifeTime_ = 0;

	// 敵の弾フラグ
	bool isEnemyBullet_ = false;

	// 死亡フラグ
	bool isDead_ = false;
};

