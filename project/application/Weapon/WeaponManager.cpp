#include "WeaponManager.h"
#include "GameContext.h"
#include "WeaponStatus.h"
#include "RangedWeapon.h"
#include "FireBall.h"

void WeaponManager::Initilaize(GameContext* context) {
	// 武器のモデル読み込み
	#pragma region fireBall
	std::unique_ptr<Entity> fireBall = std::make_unique<Entity>();
	fireBall->SetModel(context->LoadModel("Resources/Weapons","pistol.obj"));
	weaponModels_.push_back(std::move(fireBall));
	#pragma endregion

}

std::unique_ptr<RangedWeapon> WeaponManager::GetRangedWeapon(int index) {
	switch (index) {
	case int(WEAPON::FireBall):

	default:
		RangedWeaponStatus status;
		status.damage = 3;
		status.weight = 0.01f;
		status.bulletRadius = 0.3f;
		status.bulletSpeed = 0.2f;
		status.shootCoolTime = 25;
		status.weaponModel = weaponModels_[int(WEAPON::FireBall)].get();
		status.bulletDirectoryPath = "Resources/Bullets";
		status.bulletFileName = "fireBall.obj";
		return std::make_unique<FireBall>(status);
	}
}