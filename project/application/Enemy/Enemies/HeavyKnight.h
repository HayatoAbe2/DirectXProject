#pragma once
#include "Enemy.h"
#include <memory>
class HeavyKnight : public Enemy {
public:
	HeavyKnight(std::unique_ptr<Model> model, std::unique_ptr<Model> shadowModel, Vector3 pos, EnemyStatus status, std::vector<std::unique_ptr<Weapon>> rWeapons) :
		Enemy(std::move(model), std::move(shadowModel), pos, status, move(rWeapons)) {}
	void Attack(Weapon* weapon, BulletManager* bulletManager, GameContext* context, Camera* camera) override;

private:
	const int maxCombo_ = 8;
	int comboCount_ = 0;
	const int maxComboInterval_ = 8;
	int comboInterval_ = 7;

	int weaponChangeTimer_ = 300;
	int weaponNum_ = 0;
};

