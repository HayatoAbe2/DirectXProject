#include "WeaponManager.h"
#include "GameContext.h"
#include "WeaponStatus.h"
#include "Weapon.h"
#include "Pistol.h"
#include "AssaultRifle.h"
#include "Shotgun.h"
#include "FireBall.h"
#include "Wavegun.h"


void WeaponManager::Initialize(GameContext* context) {
	context_ = context;
}

std::unique_ptr<Weapon> WeaponManager::GetWeapon(int index, Rarity rarity) {
	auto model = std::make_unique<Model>();
	WeaponStatus status;

	// エンチャント付与
	status.rarity = rarity;
	for (int i = 0; i < static_cast<int>(status.rarity); ++i) {
		status.enchants.push_back(static_cast<Enchants>(context_->RandomInt(0, enchantsCount-1)));
	}

	if (index == -1) { index = context_->RandomInt(0, int(WEAPON::Wavegun)); }
	switch (index) {
	case static_cast<int>(WEAPON::Pistol):
		status.damage = 3.0f;
		status.weight = 0.1f;
		status.bulletSize = 0.5f;
		status.bulletSpeed = 0.6f;
		status.shootCoolTime = 20;
		status.bulletLifeTime = 60;
		status.knockback = 0.3f;
		status.magazineSize = 10;
		status.reloadTime = 30;
		model = context_->LoadModel("Resources/Weapons", "Pistol.obj");
		return std::make_unique<Pistol>(status, std::move(model), context_);
		break;

	case static_cast<int>(WEAPON::AssaultRifle):
		status.damage = 2.0f;
		status.weight = 0.4f;
		status.bulletSize = 0.3f;
		status.bulletSpeed = 1.0f;
		status.shootCoolTime = 6;
		status.bulletLifeTime = 60;
		status.knockback = 0.1f;
		status.magazineSize = 30;
		status.reloadTime = 75;
		model = context_->LoadModel("Resources/Weapons", "AssaultRifle.obj");
		return std::make_unique<AssaultRifle>(status, std::move(model), context_);
		break;

	case static_cast<int>(WEAPON::Shotgun):
		status.damage = 3.0f;
		status.weight = 0.3f;
		status.bulletSize = 0.4f;
		status.bulletSpeed = 0.9f;
		status.shootCoolTime = 40;
		status.bulletLifeTime = 9;
		status.knockback = 0.3f;
		status.magazineSize = 7;
		status.reloadTime = 90;
		model = context_->LoadModel("Resources/Weapons", "Shotgun.obj");
		return std::make_unique<Shotgun>(status, std::move(model), context_);
		break;

	case static_cast<int>(WEAPON::FireBall):
		status.damage = 8.0f;
		status.weight = 0.2f;
		status.bulletSize = 1.2f;
		status.bulletSpeed = 0.4f;
		status.shootCoolTime = 60;
		status.bulletLifeTime = 300;
		status.knockback = 0.5f;
		status.magazineSize = -1;
		status.reloadTime = 0;
		model = context_->LoadModel("Resources/Weapons", "Spellbook.obj");
		return std::make_unique<FireBall>(status, std::move(model), context_);
		break;

	case static_cast<int>(WEAPON::Wavegun):
		status.damage = 2.0f;
		status.weight = 0.1f;
		status.bulletSize = 0.4f;
		status.bulletSpeed = 0.4f;
		status.shootCoolTime = 10;
		status.bulletLifeTime = 180;
		status.knockback = 0.25f;
		status.magazineSize = 9;
		status.reloadTime = 50;
		model = context_->LoadModel("Resources/Weapons", "Wavegun.obj");
		return std::make_unique<Wavegun>(status, std::move(model), context_);
		break;

	default:
		status.damage = 3.0f;
		status.weight = 0.1f;
		status.bulletSize = 0.5f;
		status.bulletSpeed = 0.6f;
		status.shootCoolTime = 20;
		status.bulletLifeTime = 60;
		status.knockback = 0.3f;
		model = context_->LoadModel("Resources/Weapons", "Pistol.obj");
		return std::make_unique<Pistol>(status, std::move(model), context_);
		break;
	}
}