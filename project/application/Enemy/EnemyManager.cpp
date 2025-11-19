#include "EnemyManager.h"
#include "Entity.h"
#include "GameContext.h"
#include "Enemy.h"
#include "Camera.h"
#include "WeaponManager.h"
#include "BulletManager.h"

void EnemyManager::Initialize(GameContext* context) {
}

void EnemyManager::Update(GameContext* context, MapCheck* mapCheck, Player* player, BulletManager* bulletManager) {
	for (const auto& enemy : enemies_) {
		enemy->Update(context, mapCheck, player, bulletManager);
	}

	enemies_.erase(
		std::remove_if(enemies_.begin(), enemies_.end(),
			[](const std::unique_ptr<Enemy>& enemy) {
				return enemy->IsDead();
			}
		),
		enemies_.end()
	);
}


void EnemyManager::Draw(GameContext* context, Camera* camera) {
	for (const auto& enemy : enemies_) {
		enemy->Draw(context, camera);
	}
}

void EnemyManager::Spawn(Vector3 pos, GameContext* context,WeaponManager* weaponManager,int enemyType) {
	auto enemyModel = std::make_unique<Entity>();
	enemyModel->SetTranslate(pos);
	enemyModel->SetModel(context->LoadModel("Resources/Enemy", "enemy.obj"));

	std::unique_ptr<RangedWeapon> rangedWeapon;
	if (enemyType) {
		rangedWeapon = weaponManager->GetRangedWeapon(int(WeaponManager::WEAPON::FireBall));
	}

	auto newEnemy = std::make_unique<Enemy>(std::move(enemyModel), pos,std::move(rangedWeapon));
	enemies_.push_back(std::move(newEnemy));
}

std::vector<Enemy*> EnemyManager::GetEnemies() {
	std::vector<Enemy*> result;
	result.reserve(enemies_.size());
	for (auto& e : enemies_) {
		result.push_back(e.get());
	}
	return result;
}
