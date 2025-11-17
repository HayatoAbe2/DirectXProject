#pragma once
#include "RangedWeapon.h"
#include "WeaponStatus.h"
#include "MathUtils.h"
#include <vector>
#include <memory>

class Bullet;

class FireBall : public RangedWeapon {
public:
	FireBall(const RangedWeaponStatus& status, std::unique_ptr<Entity> renderable) { status_ = status; renderable_ = std::move(renderable); }
	int Shoot(Vector3 pos, Vector3 dir, std::vector<std::unique_ptr<Bullet>>& bullets, GameContext* context) override;
	void Update() override;

private:
	
};

