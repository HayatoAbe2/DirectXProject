#pragma once
#include "Weapon.h"
#include "MathUtils.h"
#include <vector>
#include <memory>

class BulletManager;

class Shotgun : public Weapon {
public:
	Shotgun(const WeaponStatus& status, std::unique_ptr<Model> model, GameContext* context);
	int Shoot(Vector3 pos, Vector3 dir, BulletManager* bulletManager, GameContext* context, bool isEnemyBullet) override;
	void Update() override;

private:

};

