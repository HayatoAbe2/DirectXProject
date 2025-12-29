#pragma once
#include "MathUtils.h"
#include "WeaponStatus.h"
#include <memory>

class Camera;
class GameContext;
class MapCheck;

class Bullet {
public:
	Bullet(std::unique_ptr<Model> model,const Vector3& direction,const RangedWeaponStatus& status,bool isEnemyBullet);
	virtual ~Bullet() = default;
	virtual void Update(MapCheck* mapCheck) = 0;
	virtual void Draw(GameContext* context,Camera* camera) = 0;
	virtual void Hit() = 0;

	bool IsEnemyBullet() { return isEnemyBullet_; }
	Transform GetTransform() { return model_->GetTransform(); }
	int GetDamage() { return status_.damage; }
	bool IsDead() { return isDead_; }
	bool CanErase() { return canErase_; }
	Vector3 GetPrePos() { return prePos_; }
	
protected:
	// 前フレーム場所
	Vector3 prePos_{};

	// 速度
	Vector3 velocity_{};

	// モデル
	std::unique_ptr<Model> model_ = nullptr;

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

