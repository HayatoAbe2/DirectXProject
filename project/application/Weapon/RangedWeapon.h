#pragma once
#include "MathUtils.h"
#include "WeaponStatus.h"
#include "Entity.h"
#include <vector>

class Player;
class Bullet;
class BulletManager;
class GameContext;

class RangedWeapon {
public:
	virtual int Shoot(Vector3 pos, Vector3 dir, BulletManager* bulletManager, GameContext* context,bool isEnemyBullet) = 0;
	virtual void Update() = 0;

	Entity* GetWeaponModel() { return renderable_.get(); }
	RangedWeaponStatus GetStatus() { return status_; }
	
protected:
	RangedWeaponStatus status_;
	std::unique_ptr<Entity> renderable_;
};

