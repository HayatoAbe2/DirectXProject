#pragma once
#include "Enemy.h"

class Bat : public Enemy {
public:
	Bat(std::unique_ptr<Entity> model, Vector3 pos, EnemyStatus status, std::unique_ptr<RangedWeapon> rWeapon) :
		Enemy(std::move(model), pos, status, move(rWeapon)){};
	void Attack(RangedWeapon* rangedWeapon, BulletManager* bulletManager, GameContext* context) override;

private:
};