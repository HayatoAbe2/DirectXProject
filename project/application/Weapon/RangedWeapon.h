#pragma once
#include "MathUtils.h"
#include "WeaponStatus.h"
#include "Entity.h"
#include <vector>

class Player;
class Bullet;
class GameContext;

class RangedWeapon {
public:
	virtual int Shoot(Vector3 pos, Vector3 dir, std::vector<std::unique_ptr<Bullet>>& bullets, GameContext* context) = 0;
	virtual void Update() = 0;

	Entity* GetWeaponRenderable() { return renderable_.get(); }
	
protected:
	RangedWeaponStatus status_;
	std::unique_ptr<Entity> renderable_;
};

