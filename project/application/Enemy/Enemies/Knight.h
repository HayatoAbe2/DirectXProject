#pragma once
#include "Enemy.h"
#include <memory>
class Knight : public Enemy {
public:
	Knight(std::unique_ptr<Model> model, Vector3 pos, EnemyStatus status, std::unique_ptr<Weapon> rWeapon) :
		Enemy(std::move(model), pos, status, move(rWeapon)){ }
	void Attack(Weapon* weapon, BulletManager* bulletManager, GameContext* context) override;

private:
	const int maxCombo_ = 4;
	int comboCount_ = 0;
	const int maxComboInterval_ = 10;
	int comboInterval_ = 10;
};

