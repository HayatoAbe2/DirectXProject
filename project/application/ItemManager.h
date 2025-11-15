#pragma once

class Player;
class WeaponManager;

class ItemManager {
public:
	void Initialize(WeaponManager* weaponManager);
	void Interact(Player* player);

private:
	WeaponManager* weaponManager_ = nullptr;
};

