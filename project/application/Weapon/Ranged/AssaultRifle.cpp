#include "AssaultRifle.h"
#include "Bullet.h"
#include "BulletManager.h"
#include "GameContext.h"
#include "Entity.h"

int AssaultRifle::Shoot(Vector3 pos, Vector3 dir, BulletManager* bulletManager, GameContext* context, bool isEnemyBullet) {
	auto bullet = std::make_unique<Entity>();
	bullet->SetTranslate(pos);
	bullet->SetModel(status_.bulletModel);
	bulletManager->AddBullet(std::move(bullet), dir, status_, isEnemyBullet);
	return status_.shootCoolTime;
}

void AssaultRifle::Update() {
}

