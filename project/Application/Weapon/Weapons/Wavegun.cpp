#include "Wavegun.h"
#include "Bullet.h"
#include "BulletManager.h"
#include "GameContext.h"
#include "WaveBullet.h"
#include "numbers"

Wavegun::Wavegun(const WeaponStatus& status, std::unique_ptr<Model> model, std::unique_ptr<Model> shadowModel, GameContext* context) {
	status_ = status;
	ammoLeft_ = status.magazineSize;
	model_ = std::move(model);
	shadowModel_ = std::move(shadowModel);
}

int Wavegun::Shoot(Vector3 pos, Vector3 dir, BulletManager* bulletManager, GameContext* context, Camera* camera, bool isEnemyBullet) {
	if (ammoLeft_ == 0) {
		// 弾切れ
		return 0;
	}

	auto bullet = context->LoadModel("Resources/Bullets", "gunBullet.obj");
	bullet->SetTranslate(pos);
	std::unique_ptr<WaveBullet> newBullet = std::make_unique<WaveBullet>(std::move(bullet), dir, status_, isEnemyBullet);
	newBullet->Initialize(context);
	bulletManager->AddBullet(std::move(newBullet));

	context->SoundPlay(L"Resources/Sounds/SE/shoot.mp3", false);

	if (isEnemyBullet) {
		return status_.shootCoolTime * 2;
	} else {
		camera->StartShake(0.5f, 2);
		ammoLeft_--;
		return status_.shootCoolTime;
	}
}

void Wavegun::Update() {
}

