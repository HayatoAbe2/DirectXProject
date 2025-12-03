#include "EnemyManager.h"
#include "Entity.h"
#include "GameContext.h"
#include "Enemy.h"
#include "Camera.h"
#include "WeaponManager.h"
#include "BulletManager.h"
#include "EnemyStatus.h"

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

	std::unique_ptr<RangedWeapon> rangedWeapon;
	std::vector<std::unique_ptr<RangedWeapon>> rangedWeapons;
	EnemyStatus status;
	switch (enemyType) {
	case 1:
		enemyModel->SetModel(context->LoadModel("Resources/Enemy", "enemy.obj"));
		rangedWeapon = weaponManager->GetRangedWeapon(int(WeaponManager::WEAPON::FireBall));
		status.hp = 10;
		status.radius = 0.5f;
		status.moveSpeed = 0.15f;
		status.defaultSearchRadius = 8.0f;
		status.loseSightRadius = 30.0f;
		status.loseSightTime = 180;
		status.moveTime = 60;
		status.stopTime = 5;
		status.canFly = true;
		break;

	case 2:
		enemyModel->SetModel(context->LoadModel("Resources/Enemy", "knight.obj"));
		rangedWeapon = weaponManager->GetRangedWeapon(int(WeaponManager::WEAPON::AssaultRifle));
		status.hp = 20;
		status.radius = 0.75f;
		status.moveSpeed = 0.07f;
		status.defaultSearchRadius = 12.0f;
		status.loseSightRadius = 20.0f;
		status.loseSightTime = 300;
		status.moveTime = 60;
		status.stopTime = 60;
		status.canFly = false;
		break;

	case 3:
		enemyModel->SetModel(context->LoadModel("Resources/Enemy", "knight.obj"));
		rangedWeapons.push_back(weaponManager->GetRangedWeapon(int(WeaponManager::WEAPON::AssaultRifle)));
		rangedWeapons.push_back(weaponManager->GetRangedWeapon(int(WeaponManager::WEAPON::FireBall)));
		status.hp = 100;
		status.radius = 0.75f;
		status.moveSpeed = 0.07f;
		status.defaultSearchRadius = 12.0f;
		status.loseSightRadius = 20.0f;
		status.loseSightTime = 300;
		status.moveTime = 60;
		status.stopTime = 60;
		status.canFly = false;
		break;
	
	default:
		enemyModel->SetModel(context->LoadModel("Resources/Enemy", "enemy.obj"));
		rangedWeapon = weaponManager->GetRangedWeapon(int(WeaponManager::WEAPON::FireBall));
		status.hp = 10;
		status.radius = 0.5f;
		status.moveSpeed = 0.15f;
		status.defaultSearchRadius = 8.0f;
		status.loseSightRadius = 30.0f;
		status.loseSightTime = 180;
		status.moveTime = 60;
		status.stopTime = 5;
		status.canFly = true;
		break;
	}
	if (rangedWeapons.empty()) {
		auto newEnemy = std::make_unique<Enemy>(std::move(enemyModel), pos, status, std::move(rangedWeapon));
		enemies_.push_back(std::move(newEnemy));
	} else {
		auto newEnemy = std::make_unique<Enemy>(std::move(enemyModel), pos, status, std::move(rangedWeapons));
		enemies_.push_back(std::move(newEnemy));
	}
}

void EnemyManager::Reset() {
	enemies_.clear();
}

std::vector<Enemy*> EnemyManager::GetEnemies() {
	std::vector<Enemy*> result;
	result.reserve(enemies_.size());
	for (auto& e : enemies_) {
		result.push_back(e.get());
	}
	return result;
}
