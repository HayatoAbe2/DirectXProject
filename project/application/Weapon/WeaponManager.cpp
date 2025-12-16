#include "WeaponManager.h"
#include "GameContext.h"
#include "WeaponStatus.h"
#include "RangedWeapon.h"
#include "FireBall.h"
#include "AssaultRifle.h"
#include "Shotgun.h"
#include "Pistol.h"

void WeaponManager::Initilaize(GameContext* context) {
	context_ = context;
}

std::unique_ptr<RangedWeapon> WeaponManager::GetRangedWeapon(int index) {
	auto model = std::make_unique<Model>();
	RangedWeaponStatus status;

	switch (index) {
		case int(WEAPON::FireBall) :
		status.damage = 8;
		status.weight = 0.2f;
		status.bulletSize = 1.2f;
		status.bulletSpeed = 0.4f;
		status.shootCoolTime = 60;
		status.bulletLifeTime = 300;
		model = context_->LoadModel("Resources/Weapons", "Spellbook.obj");
		return std::make_unique<FireBall>(status, std::move(model),context_);
		break;

		case int(WEAPON::AssaultRifle) :
		status.damage = 2;
		status.weight = 0.4f;
		status.bulletSize = 0.3f;
		status.bulletSpeed = 1.0f;
		status.shootCoolTime = 6;
		status.bulletLifeTime = 120;
		model = context_->LoadModel("Resources/Weapons", "AssaultRifle.obj");
		return std::make_unique<AssaultRifle>(status, std::move(model), context_);
		break;

		case int(WEAPON::Shotgun):
		status.damage = 3;
		status.weight = 0.3f;
		status.bulletSize = 0.4f;
		status.bulletSpeed = 0.9f;
		status.shootCoolTime = 40;
		status.bulletLifeTime = 9;
		model = context_->LoadModel("Resources/Weapons", "Shotgun.obj");
		return std::make_unique<Shotgun>(status, std::move(model), context_);
		break;

	default:
		status.damage = 3;
		status.weight = 0.1f;
		status.bulletSize = 0.5f;
		status.bulletSpeed = 0.6f;
		status.shootCoolTime = 20;
		status.bulletLifeTime = 60;
		model = context_->LoadModel("Resources/Weapons", "Pistol.obj");
		return std::make_unique<Pistol>(status, std::move(model), context_);
		break;
	}
}