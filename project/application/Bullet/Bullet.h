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
	virtual void Update(MapCheck* mapCheck) = 0;
	virtual void Draw(GameContext* context,Camera* camera) = 0;
	virtual void Hit() = 0;

	bool IsEnemyBullet() { return isEnemyBullet_; }
	Transform GetTransform() { return model_->GetTransform(); }
	int GetDamage() { return status_.damage; }
	bool IsDead() { return isDead_; }
	bool CanErase() { return canErase_; }
	
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

	// 死亡フラグ(判定やめる)
	bool isDead_ = false;

	// 弾リストから削除可能
	bool canErase_ = false;
};

