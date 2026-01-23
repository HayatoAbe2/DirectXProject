#pragma once
#include "MathUtils.h"
#include "WeaponStatus.h"
#include <vector>

class BulletManager;
class GameContext;

class Weapon {
public:
	virtual ~Weapon() = default;
	virtual int Shoot(Vector3 pos, Vector3 dir, BulletManager* bulletManager, GameContext* context,bool isEnemyBullet) = 0;
	virtual void Update() = 0;

	Model* GetWeaponModel() { return model_.get(); }
	const WeaponStatus& GetStatus() const { return status_; }
	
protected:
	WeaponStatus status_;
	std::unique_ptr<Model> model_;
};

