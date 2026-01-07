#pragma once

#include "Rarity.h"
#include <vector>
#include <memory>

class GameContext;
class RangedWeapon;

class WeaponManager {
public:
	void Initialize(GameContext* context);

	std::unique_ptr<RangedWeapon> GetRangedWeapon(int index,Rarity rarity = Rarity::Common);
	
	enum class WEAPON {
		Pistol,
		AssaultRifle,
		Shotgun,
		FireBall,
		Wavegun,
	};
private:
	GameContext* context_ = nullptr;
};

