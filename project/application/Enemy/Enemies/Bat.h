#pragma once
#include "Enemy.h"
#include <memory>

class Bat : public Enemy {
public:
	Bat(std::unique_ptr<Model> model, Vector3 pos, EnemyStatus status, std::unique_ptr<RangedWeapon> rWeapon) :
		Enemy(std::move(model), pos, status, move(rWeapon)){};
	void Attack(RangedWeapon* rangedWeapon, BulletManager* bulletManager, GameContext* context) override;

private:
};