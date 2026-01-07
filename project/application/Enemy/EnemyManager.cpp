#include "EnemyManager.h"
#include "GameContext.h"
#include "Enemy.h"
#include "Camera.h"
#include "WeaponManager.h"
#include "BulletManager.h"
#include "EnemyStatus.h"

#include "Bat.h"
#include "Knight.h"
#include "HeavyKnight.h"
#include "RedBat.h"
#include <fstream>
#include <sstream>

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

void EnemyManager::Spawn(Vector3 pos, GameContext* context, WeaponManager* weaponManager, int enemyType){
   auto enemyModel = std::make_unique<Model>();
   enemyModel->SetTranslate(pos);

   std::unique_ptr<RangedWeapon> rangedWeapon = nullptr;
   std::vector<std::unique_ptr<RangedWeapon>> rangedWeapons;
   EnemyStatus status;

   switch (enemyType) {
   case 1:
       enemyModel = context->LoadModel("Resources/Enemy", "bat.obj");
       rangedWeapon = weaponManager->GetRangedWeapon(int(WeaponManager::WEAPON::FireBall));
       status.hp = 10;
       status.radius = 0.5f;
       status.moveSpeed = 0.15f;
       status.defaultSearchRadius = 80.0f;
       status.loseSightRadius = 100.0f;
       status.loseSightTime = 180;
       status.moveTime = 60;
       status.stopTime = 5;
       status.stunResist = 0;
       status.canFly = true;

       enemies_.push_back(std::make_unique<Bat>(std::move(enemyModel), pos, status, std::move(rangedWeapon)));
       break;

   case 2:
       enemyModel = context->LoadModel("Resources/Enemy", "knight.obj");
       rangedWeapon = weaponManager->GetRangedWeapon(int(WeaponManager::WEAPON::AssaultRifle));
       status.hp = 20;
       status.radius = 0.9f;
       status.moveSpeed = 0.07f;
       status.defaultSearchRadius = 80.0f;
       status.loseSightRadius = 100.0f;
       status.loseSightTime = 300;
       status.moveTime = 60;
       status.stopTime = 60;
       status.stunResist = 2;
       status.canFly = false;

       enemies_.push_back(std::make_unique<Knight>(std::move(enemyModel), pos, status, std::move(rangedWeapon)));
       break;

   case 3:
       enemyModel = context->LoadModel("Resources/Enemy", "knight2.obj");
       rangedWeapons.push_back(weaponManager->GetRangedWeapon(int(WeaponManager::WEAPON::AssaultRifle)));
       rangedWeapons.push_back(weaponManager->GetRangedWeapon(int(WeaponManager::WEAPON::AssaultRifle)));
       rangedWeapons.push_back(weaponManager->GetRangedWeapon(int(WeaponManager::WEAPON::FireBall)));
       status.hp = 150;
       status.radius = 1.5f;
       status.moveSpeed = 0.07f;
       status.defaultSearchRadius = 100.0f;
       status.loseSightRadius = 300.0f;
       status.loseSightTime = 300;
       status.moveTime = 60;
       status.stopTime = 60;
       status.stunResist = 30;
       status.canFly = false;

       enemies_.push_back(std::make_unique<HeavyKnight>(std::move(enemyModel), pos, status, std::move(rangedWeapons)));
       break;

   default:
       enemyModel = context->LoadModel("Resources/Enemy", "bat2.obj");
       rangedWeapons.push_back(weaponManager->GetRangedWeapon(int(WeaponManager::WEAPON::FireBall)));
       rangedWeapons.push_back(weaponManager->GetRangedWeapon(int(WeaponManager::WEAPON::Pistol)));
       rangedWeapons.push_back(weaponManager->GetRangedWeapon(int(WeaponManager::WEAPON::Wavegun)));
       status.hp = 110;
       status.radius = 0.75f;
       status.moveSpeed = 0.15f;
       status.defaultSearchRadius = 100.0f;
       status.loseSightRadius = 300.0f;
       status.loseSightTime = 180;
       status.moveTime = 60; 
       status.stopTime = 5;
       status.stunResist = 30;
       status.canFly = true;

       enemies_.push_back(std::make_unique<RedBat>(std::move(enemyModel), pos, status, std::move(rangedWeapons)));
       break;
   }
}

void EnemyManager::Reset() {
	enemies_.clear();
}

void EnemyManager::LoadCSV(std::string filePath, float tileSize, GameContext* context, WeaponManager* weaponManager) {
    std::ifstream file(filePath);
    std::string line;

    assert(file.is_open());

    std::getline(file, line); // 最初の行をスキップ

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string enemyStr, xStr, zStr;

        std::getline(ss, enemyStr, ',');
        std::getline(ss, xStr, ',');
        std::getline(ss, zStr, ',');

        int enemyNum = std::stoi(enemyStr);
        float x = std::stof(xStr);
        float z = std::stof(zStr);

        Vector3 pos = Vector3{ x * tileSize, 0, z * tileSize };
        Spawn(pos, context,weaponManager, enemyNum);
    }
}

std::vector<Enemy*> EnemyManager::GetEnemies() {
	std::vector<Enemy*> result;
	result.reserve(enemies_.size());
	for (auto& e : enemies_) {
		result.push_back(e.get());
	}
	return result;
}
