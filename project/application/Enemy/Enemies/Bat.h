#pragma once
#include "Enemy.h"
#include <memory>

class Bat : public Enemy {
public:
	Bat(std::unique_ptr<Model> model, Vector3 pos, EnemyStatus status, std::unique_ptr<Weapon> rWeapon) :
		Enemy(std::move(model), pos, status, move(rWeapon)){};
	void Attack(Weapon* weapon, BulletManager* bulletManager, GameContext* context) override;

private:
};