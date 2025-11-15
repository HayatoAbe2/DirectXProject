#pragma once
#include "MathUtils.h"
#include "WeaponStatus.h"
#include <vector>

class Player;
class Entity;
class Bullet;
class GameContext;

class RangedWeapon {
public:
	virtual void Shoot(Vector3 pos, Vector3 dir, std::vector<std::unique_ptr<Bullet>>& bullets, GameContext* context) = 0;
	virtual void Update() = 0;

	Entity* GetWeaponModel() { return status_.weaponModel; }
	
protected:
	RangedWeaponStatus status_;
	int shootCoolTimer_;
};

