#include "WeaponManager.h"
#include "GameContext.h"
#include "WeaponStatus.h"
#include "RangedWeapon.h"
#include "FireBall.h"

void WeaponManager::Initilaize(GameContext* context) {
	// 武器のモデル読み込み
	#pragma region weapons
	weaponModels_.push_back(context->LoadModel("Resources/Weapons", "pistol.obj"));
	#pragma endregion

	// 弾のモデル読み込み
	#pragma region bullets
	bulletModels_.push_back(context->LoadModel("Resources/Bullets", "fireBall.obj"));
	#pragma endregion

}

std::unique_ptr<RangedWeapon> WeaponManager::GetRangedWeapon(int index) {
	auto renderable = std::make_unique<Entity>();
	
	switch (index) {
	case int(WEAPON::FireBall):

	default:
		RangedWeaponStatus status;
		status.damage = 3;
		status.weight = 0.01f;
		status.bulletSize = 1.0f;
		status.bulletSpeed = 0.2f;
		status.shootCoolTime = 25;
		status.bulletModel = bulletModels_[int(WEAPON::FireBall)];
		renderable->SetModel(weaponModels_[int(WEAPON::FireBall)]);

		return std::make_unique<FireBall>(status,std::move(renderable));
	}
}