#pragma once
#include "Enemy.h"
class HeavyKnight : public Enemy {
public:
	HeavyKnight(std::unique_ptr<Entity> model, Vector3 pos, EnemyStatus status, std::vector<std::unique_ptr<RangedWeapon>> rWeapons) :
		Enemy(std::move(model), pos, status, move(rWeapons)) {}
	void Attack(RangedWeapon* rangedWeapon, BulletManager* bulletManager, GameContext* context) override;

private:
	const int maxCombo_ = 8;
	int comboCount_ = 0;
	const int maxComboInterval_ = 8;
	int comboInterval_ = 7;

	int weaponChangeTimer_ = 300;
	int weaponNum_ = 0;
};

