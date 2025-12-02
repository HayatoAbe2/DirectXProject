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
		AssaultRifle,
		Shotgun,
	};
private:
	std::vector<std::shared_ptr<Model>> weaponModels_;
	std::vector<std::shared_ptr<Model>> bulletModels_;
	GameContext* context_ = nullptr;
};

