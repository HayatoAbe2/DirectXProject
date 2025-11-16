#pragma once
#include "RangedWeapon.h"
#include "WeaponStatus.h"
#include "MathUtils.h"
#include <vector>
#include <memory>

class Bullet;

class FireBall : public RangedWeapon {
public:
	FireBall(const RangedWeaponStatus& status) { status_ = status; }
	int Shoot(Vector3 pos, Vector3 dir, std::vector<std::unique_ptr<Bullet>>& bullets, GameContext* context) override;
	void Update() override;

private:
	
};

