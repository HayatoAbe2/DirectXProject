#pragma once

#include "Rarity.h"
#include <vector>
#include <memory>

class GameContext;
class Weapon;

class WeaponManager {
public:
	void Initialize(GameContext* context);

	std::unique_ptr<Weapon> GetWeapon(int index,Rarity rarity = Rarity::Common);
	
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

