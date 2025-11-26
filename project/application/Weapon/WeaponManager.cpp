#include "WeaponManager.h"
#include "GameContext.h"
#include "WeaponStatus.h"
#include "RangedWeapon.h"
#include "FireBall.h"
#include "AssaultRifle.h"

void WeaponManager::Initilaize(GameContext* context) {
	context_ = context;

	// 武器のモデル読み込み
#pragma region weapons
	weaponModels_.push_back(context->LoadModel("Resources/Weapons", "pistol.obj"));
	weaponModels_.push_back(context->LoadModel("Resources/Weapons", "assaultRifle.obj"));
#pragma endregion

	// 弾のモデル読み込み
#pragma region bullets
	bulletModels_.push_back(context->LoadModel("Resources/Bullets", "fireBall.obj"));
	bulletModels_.push_back(context->LoadModel("Resources/Bullets", "gunBullet.obj"));
#pragma endregion

}

std::unique_ptr<RangedWeapon> WeaponManager::GetRangedWeapon(int index) {
	auto model = std::make_unique<Entity>();
	RangedWeaponStatus status;

	switch (index) {
	case 0:
		status.damage = 3;
		status.weight = 0.2f;
		status.bulletSize = 0.8f;
		status.bulletSpeed = 0.4f;
		status.shootCoolTime = 25;
		status.bulletLifeTime = 300;
		status.bulletModel = bulletModels_[int(WEAPON::FireBall)];
		model->SetModel(weaponModels_[int(WEAPON::FireBall)]);
		return std::make_unique<FireBall>(status, std::move(model),context_);
		break;

	default:
		status.damage = 2;
		status.weight = 0.4f;
		status.bulletSize = 0.4f;
		status.bulletSpeed = 1.0f;
		status.shootCoolTime = 6;
		status.bulletLifeTime = 120;
		status.bulletModel = bulletModels_[int(WEAPON::AssaultRifle)];
		model->SetModel(weaponModels_[int(WEAPON::AssaultRifle)]);
		return std::make_unique<AssaultRifle>(status, std::move(model));
		break;
	}
}