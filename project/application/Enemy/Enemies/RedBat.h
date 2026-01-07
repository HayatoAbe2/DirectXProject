#pragma once
#include "Enemy.h"
#include <memory>

class RedBat : public Enemy {
public:
	RedBat(std::unique_ptr<Model> model, Vector3 pos, EnemyStatus status, std::vector<std::unique_ptr<RangedWeapon>> rWeapons) :
		Enemy(std::move(model), pos, status, move(rWeapons)) {
	};
	void Attack(RangedWeapon* rangedWeapon, BulletManager* bulletManager, GameContext* context) override;

private:
	const int maxCombo_ = 8;
	int comboCount_ = 0;
	const int maxComboInterval_ = 8;
	int comboInterval_ = 7;

	int weaponChangeTimer_ = 300;
	int weaponNum_ = 0;
};