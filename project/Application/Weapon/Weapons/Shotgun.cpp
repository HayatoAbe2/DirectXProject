#include "Shotgun.h"
#include "Bullet.h"
#include "BulletManager.h"
#include "GameContext.h"
#include "SpreadBullet.h"
#include "numbers"

Shotgun::Shotgun(const WeaponStatus& status, std::unique_ptr<Model> model,GameContext* context) {
	status_ = status;
	ammoLeft_ = status.magazineSize;
	model_ = std::move(model);
}

int Shotgun::Shoot(Vector3 pos, Vector3 dir, BulletManager* bulletManager, GameContext* context, bool isEnemyBullet) {
	if (ammoLeft_ == 0) {
		// 弾切れ
		return 0;
	}

	for (int i = -2; i <= 2; ++i) {
		auto bullet = context->LoadModel("Resources/Bullets", "gunBullet.obj");
		bullet->SetTranslate(pos);

		Vector3 rotatedDir = TransformVector(dir, MakeRotateYMatrix(float(std::numbers::pi) / 16.0f * i));
		std::unique_ptr<SpreadBullet> newBullet = std::make_unique<SpreadBullet>(std::move(bullet), rotatedDir, status_, isEnemyBullet);
		newBullet->Initialize(context);

		bulletManager->AddBullet(std::move(newBullet));
	}
	context->SoundPlay(L"Resources/Sounds/SE/shoot.mp3", false);

	if (isEnemyBullet) {
		return status_.shootCoolTime * 2;
	} else {
		ammoLeft_--;
		return status_.shootCoolTime;
	}
}

void Shotgun::Update() {
}

