#include "EnemyManager.h"
#include "Entity.h"
#include "GameContext.h"
#include "Enemy.h"
#include "Camera.h"

void EnemyManager::Initialize(GameContext* context) {
	Spawn({ 5,0,5 }, context);
}

void EnemyManager::Update(GameContext* context, MapCheck* mapCheck, Player* player, std::vector<std::unique_ptr<Bullet>>& bullets) {
	for (const auto& enemy : enemies_) {
		enemy->Update(context, mapCheck, player, bullets);
	}
}


void EnemyManager::Draw(GameContext* context, Camera* camera) {
	for (const auto& enemy : enemies_) {
		enemy->Draw(context, camera);
	}
}

void EnemyManager::Spawn(Vector3 pos, GameContext* context) {
	auto enemyModel = std::make_unique<Entity>();
	enemyModel->SetTranslate(pos);
	enemyModel->SetModel(context->LoadModel("Resources/Enemy", "enemy.obj"));
	auto newEnemy = std::make_unique<Enemy>(std::move(enemyModel), pos);
	enemies_.push_back(std::move(newEnemy));
}