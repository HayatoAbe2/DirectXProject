#include "ItemManager.h"
#include "Player.h"
#include "WeaponManager.h"
#include "RangedWeapon.h"

void ItemManager::Initialize(WeaponManager* weaponManager) {
	weaponManager_ = weaponManager;
}

void ItemManager::Interact(Player* player) {
	auto weapon = weaponManager_->GetRangedWeapon(0);
	player->SetWeapon(std::move(weapon));
}
