#include "Pistol.h"
#include "Bullet.h"
#include "BulletManager.h"
#include "GameContext.h"
#include "Entity.h"
#include "RapidBullet.h"

Pistol::Pistol(const RangedWeaponStatus& status, std::unique_ptr<Entity> model) {
	status_ = status;
	model_ = std::move(model);
}

int Pistol::Shoot(Vector3 pos, Vector3 dir, BulletManager* bulletManager, GameContext* context, bool isEnemyBullet) {
	auto bullet = std::make_unique<Entity>();
	bullet->SetTranslate(pos);
	bullet->SetModel(status_.bulletModel);
	std::unique_ptr<RapidBullet> newBullet = std::make_unique<RapidBullet>(std::move(bullet), dir, status_, isEnemyBullet);
	newBullet->Initialize(context);

	bulletManager->AddBullet(std::move(newBullet));

	context->SoundPlay(L"Resources/Sounds/SE/shoot.mp3", false);

	if (isEnemyBullet) {
		return status_.shootCoolTime * 2;
	} else {
		return status_.shootCoolTime;
	}
}

void Pistol::Update() {
}

