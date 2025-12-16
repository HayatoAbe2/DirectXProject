#include "FireBall.h"
#include "Bullet.h"
#include "BulletManager.h"
#include "GameContext.h"
#include "FireBullet.h"

FireBall::FireBall(const RangedWeaponStatus& status, std::unique_ptr<Model> model, GameContext* context) {
	status_ = status;
	model_ = std::move(model);
}

int FireBall::Shoot(Vector3 pos, Vector3 dir, BulletManager* bulletManager, GameContext* context, bool isEnemyBullet) {
	auto bullet = std::make_unique<Model>();
	bullet->SetTranslate(pos);
	bullet = context->LoadModel("Resources/Bullets", "gunBullet.obj");
	std::unique_ptr<FireBullet> newBullet = std::make_unique<FireBullet>(std::move(bullet), dir, status_, isEnemyBullet);
	newBullet->Initialize(context);

	bulletManager->AddBullet(std::move(newBullet));

	context->SoundPlay(L"Resources/Sounds/SE/fire.mp3", false);

	if (isEnemyBullet) {
		return status_.shootCoolTime * 2;
	} else {
		return status_.shootCoolTime;
	}
}

void FireBall::Update() {
}

