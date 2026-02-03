#include "FireBall.h"
#include "Bullet.h"
#include "BulletManager.h"
#include "GameContext.h"
#include "FireBullet.h"

FireBall::FireBall(const WeaponStatus& status, std::unique_ptr<Model> model, std::unique_ptr<Model> shadowModel, GameContext* context) {
	status_ = status;
	ammoLeft_ = status.magazineSize;
	model_ = std::move(model);
	shadowModel_ = std::move(shadowModel_);
}

int FireBall::Shoot(Vector3 pos, Vector3 dir, BulletManager* bulletManager, GameContext* context, Camera* camera, bool isEnemyBullet) {
	auto bullet = context->LoadModel("Resources/Bullets", "gunBullet.obj");
	bullet->SetTranslate(pos);
	std::unique_ptr<FireBullet> newBullet = std::make_unique<FireBullet>(std::move(bullet), dir, status_, isEnemyBullet);
	newBullet->Initialize(context);

	bulletManager->AddBullet(std::move(newBullet));

	context->SoundPlay(L"Resources/Sounds/SE/fire.mp3", false);

	if (isEnemyBullet) {
		return status_.shootCoolTime * 2;
	} else {
		// 弾は減らない

		return status_.shootCoolTime;
	}
}

void FireBall::Update() {
}

