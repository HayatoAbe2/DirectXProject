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

	// 武器のモデル読み込み
#pragma region weapons
	weaponModels_.push_back(context->LoadModel("Resources/Weapons", "Spellbook.obj"));
	weaponModels_.push_back(context->LoadModel("Resources/Weapons", "AssaultRifle.obj"));
	weaponModels_.push_back(context->LoadModel("Resources/Weapons", "Shotgun.obj"));
	weaponModels_.push_back(context->LoadModel("Resources/Weapons", "Pistol.obj"));
#pragma endregion

	// 弾のモデル読み込み
#pragma region bullets
	bulletModels_.push_back(context->LoadModel("Resources/Bullets", "fireBall.obj"));
	bulletModels_.push_back(context->LoadModel("Resources/Bullets", "gunBullet.obj"));
	bulletModels_.push_back(context->LoadModel("Resources/Bullets", "gunBullet.obj"));
	bulletModels_.push_back(context->LoadModel("Resources/Bullets", "gunBullet.obj"));
#pragma endregion

}

std::unique_ptr<RangedWeapon> WeaponManager::GetRangedWeapon(int index) {
	auto model = std::make_unique<Entity>();
	RangedWeaponStatus status;

	switch (index) {
		case int(WEAPON::FireBall) :
		status.damage = 8;
		status.weight = 0.2f;
		status.bulletSize = 1.2f;
		status.bulletSpeed = 0.4f;
		status.shootCoolTime = 60;
		status.bulletLifeTime = 300;
		status.bulletModel = bulletModels_[int(WEAPON::FireBall)];
		model->SetModel(weaponModels_[int(WEAPON::FireBall)]);
		return std::make_unique<FireBall>(status, std::move(model),context_);
		break;

		case int(WEAPON::AssaultRifle) :
		status.damage = 2;
		status.weight = 0.4f;
		status.bulletSize = 0.3f;
		status.bulletSpeed = 1.0f;
		status.shootCoolTime = 6;
		status.bulletLifeTime = 120;
		status.bulletModel = bulletModels_[int(WEAPON::AssaultRifle)];
		model->SetModel(weaponModels_[int(WEAPON::AssaultRifle)]);
		return std::make_unique<AssaultRifle>(status, std::move(model));
		break;

		case int(WEAPON::Shotgun):
		status.damage = 3;
		status.weight = 0.3f;
		status.bulletSize = 0.4f;
		status.bulletSpeed = 0.9f;
		status.shootCoolTime = 40;
		status.bulletLifeTime = 9;
		status.bulletModel = bulletModels_[int(WEAPON::Shotgun)];
		model->SetModel(weaponModels_[int(WEAPON::Shotgun)]);
		return std::make_unique<Shotgun>(status, std::move(model));
		break;

	default:
		status.damage = 3;
		status.weight = 0.1f;
		status.bulletSize = 0.5f;
		status.bulletSpeed = 0.6f;
		status.shootCoolTime = 20;
		status.bulletLifeTime = 60;
		status.bulletModel = bulletModels_[int(WEAPON::Pistol)];
		model->SetModel(weaponModels_[int(WEAPON::Pistol)]);
		return std::make_unique<Pistol>(status, std::move(model));
		break;
	}
}