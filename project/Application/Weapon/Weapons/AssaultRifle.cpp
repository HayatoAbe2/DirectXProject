#include "AssaultRifle.h"
#include "Bullet.h"
#include "BulletManager.h"
#include "GameContext.h"
#include "NormalBullet.h"

AssaultRifle::AssaultRifle(const WeaponStatus& status, std::unique_ptr<Model> model,GameContext* context) {
	status_ = status;
	ammoLeft_ = status.magazineSize;
	model_ = std::move(model);
}

int AssaultRifle::Shoot(Vector3 pos, Vector3 dir, BulletManager* bulletManager, GameContext* context, bool isEnemyBullet) {
	if (ammoLeft_ == 0) {
		// 弾切れ
		return 0;
	}

	auto bullet = context->LoadModel("Resources/Bullets", "gunBullet.obj");
	bullet->SetTranslate(pos);
	std::unique_ptr<NormalBullet> newBullet = std::make_unique<NormalBullet>(std::move(bullet), dir, status_, isEnemyBullet);
	newBullet->Initialize(context);
	bulletManager->AddBullet(std::move(newBullet));

	context->SoundPlay(L"Resources/Sounds/SE/shoot.mp3", false);

	if (isEnemyBullet) {
		return status_.shootCoolTime * 2;
	} else {
		ammoLeft_--;
		return status_.shootCoolTime;
	}
}

void AssaultRifle::Update() {
	if (isReloading_) {
		reloadTimer_--;

		if (reloadTimer_ <= 0) {
			ammoLeft_ = status_.magazineSize;
			isReloading_ = false;
		}
	}
}

