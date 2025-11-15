#pragma once
#include "Entity.h"

#include <vector>
#include <memory>

class GameContext;
class RangedWeapon;

class WeaponManager {
public:
	void Initilaize(GameContext* context);

	std::unique_ptr<RangedWeapon> GetRangedWeapon(int index);
	
	enum class WEAPON {
		FireBall,
	};
private:
	std::vector<std::unique_ptr<Entity>> weaponModels_;
};

