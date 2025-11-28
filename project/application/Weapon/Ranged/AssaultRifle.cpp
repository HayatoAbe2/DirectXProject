#include "AssaultRifle.h"
#include "Bullet.h"
#include "BulletManager.h"
#include "GameContext.h"
#include "Entity.h"
#include "NormalBullet.h"

AssaultRifle::AssaultRifle(const RangedWeaponStatus& status, std::unique_ptr<Entity> model) {
	status_ = status;
	model_ = std::move(model);
}

int AssaultRifle::Shoot(Vector3 pos, Vector3 dir, BulletManager* bulletManager, GameContext* context, bool isEnemyBullet) {
	auto bullet = std::make_unique<Entity>();
	bullet->SetTranslate(pos);
	bullet->SetModel(status_.bulletModel);
	std::unique_ptr<NormalBullet> newBullet = std::make_unique<NormalBullet>(std::move(bullet), dir, status_, isEnemyBullet);
	newBullet->Initialize(context);
	bulletManager->AddBullet(std::move(newBullet));
	return status_.shootCoolTime;
}

void AssaultRifle::Update() {
}

