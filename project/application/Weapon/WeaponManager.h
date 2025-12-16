#pragma once

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
		Pistol,
	};
private:
	GameContext* context_ = nullptr;
};

