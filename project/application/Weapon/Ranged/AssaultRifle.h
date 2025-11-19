#pragma once
#include "RangedWeapon.h"
#include "MathUtils.h"
#include <vector>
#include <memory>

class BulletManager;

class AssaultRifle : public RangedWeapon {
public:
	AssaultRifle(const RangedWeaponStatus& status, std::unique_ptr<Entity> renderable) { status_ = status; renderable_ = std::move(renderable); }
	int Shoot(Vector3 pos, Vector3 dir, BulletManager* bulletManager, GameContext* context, bool isEnemyBullet) override;
	void Update() override;

private:

};

