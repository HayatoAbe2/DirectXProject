#pragma once
#include "Enemy.h"
class Knight : public Enemy {
public:
	Knight(std::unique_ptr<Entity> model, Vector3 pos, EnemyStatus status, std::unique_ptr<RangedWeapon> rWeapon) :
		Enemy(std::move(model), pos, status, move(rWeapon)){ }
	void Attack(RangedWeapon* rangedWeapon, BulletManager* bulletManager, GameContext* context) override;

private:
	const int maxCombo_ = 4;
	int comboCount_ = 0;
	const int maxComboInterval_ = 10;
	int comboInterval_ = 10;
};

