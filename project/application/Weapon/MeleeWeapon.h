#pragma once
#include "MathUtils.h"
#include "WeaponStatus.h"
#include "Entity.h"
#include <vector>

class Player;
class Bullet;
class BulletManager;
class GameContext;

class MeleeWeapon {
	virtual int Attack(Vector3 pos, Vector3 dir, BulletManager* bulletManager, GameContext* context, bool isEnemyAttack) = 0;
	virtual void Update() = 0;

	Entity* GetWeaponModel() { return model_.get(); }
	MeleeWeaponStatus GetStatus() { return status_; }

protected:
	MeleeWeaponStatus status_;
	std::unique_ptr<Entity> model_;
};

