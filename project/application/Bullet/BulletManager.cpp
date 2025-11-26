#include "BulletManager.h"
#include "EffectManager.h"
#include "MapCheck.h"
#include "Bullet.h"
#include "Entity.h"

void BulletManager::Initialize(EffectManager* effectManager) {
}

void BulletManager::Update(MapCheck* mapCheck) {
	for (const auto& bullet : bullets_) {
		bullet->Update(mapCheck);
	}
	bullets_.erase(
		std::remove_if(bullets_.begin(), bullets_.end(),
			[](const std::unique_ptr<Bullet>& bullet) {
				return bullet->IsDead();
			}
		),
		bullets_.end()
	);
}

void BulletManager::Draw(GameContext* context, Camera* camera) {
	for (const auto& bullet : bullets_) {
		bullet->Draw(context, camera);
	}
}

void BulletManager::AddBullet(std::unique_ptr<Bullet> newBullet) {
	bullets_.push_back(std::move(newBullet));
}

std::vector<Bullet*> BulletManager::GetBullets() {
	std::vector<Bullet*> bullets; bullets_;
	for (const auto& bullet : bullets_) {
		bullets.push_back(bullet.get());
	}
	return bullets;
}
