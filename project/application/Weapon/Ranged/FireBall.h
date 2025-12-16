#pragma once
#include "RangedWeapon.h"
#include "MathUtils.h"
#include <vector>
#include <memory>

class BulletManager;

class FireBall : public RangedWeapon {
public:
	FireBall(const RangedWeaponStatus& status, std::unique_ptr<Model> model,GameContext* context);
	int Shoot(Vector3 pos, Vector3 dir, BulletManager* bulletManager, GameContext* context, bool isEnemyBullet) override;
	void Update() override;

private:
};

