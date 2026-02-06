#include "Pistol.h"
#include "Bullet.h"
#include "BulletManager.h"
#include "GameContext.h"
#include "RapidBullet.h"

Pistol::Pistol(const WeaponStatus& status, std::unique_ptr<Model> model, std::unique_ptr<Model> shadowModel, GameContext* context) {
	status_ = status;
	ammoLeft_ = status.magazineSize;
	model_ = std::move(model);
	shadowModel_ = std::move(shadowModel);
}

int Pistol::Shoot(Vector3 pos, Vector3 dir, BulletManager* bulletManager, GameContext* context, Camera* camera, bool isEnemyBullet) {
	auto bullet = context->LoadModel("Resources/Bullets", "gunBullet.obj");
	bullet->SetTranslate(pos);
	std::unique_ptr<RapidBullet> newBullet = std::make_unique<RapidBullet>(std::move(bullet), dir, status_, isEnemyBullet);
	newBullet->Initialize(context);

	bulletManager->AddBullet(std::move(newBullet));

	context->SoundPlay(L"Resources/Sounds/SE/shoot.mp3", false);

	if (isEnemyBullet) {
		return status_.shootCoolTime * 2;
	} else {
		camera->StartShake(0.2f, 2);
		ammoLeft_--;
		return status_.shootCoolTime;
	}
}

void Pistol::Update() {
}

