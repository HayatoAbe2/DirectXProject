#pragma once
#include "MathUtils.h"
#include "Camera.h"
#include "Enemy.h"
#include <vector>
#include <memory>

class GameContext;
class MapCheck;
class Player;
class BulletManager;
class WeaponManager;

class EnemyManager {
public:
	void Initialize(GameContext* context);
	void Update(GameContext* context, MapCheck* mapCheck, Player* player, BulletManager* bulletManager);
	void Draw(GameContext* context, Camera* camera);
	void Spawn(Vector3 pos,GameContext* context,WeaponManager* weaponManager, int enemyType);
	void Reset();

	std::vector<Enemy*> GetEnemies();
private:
	std::vector<std::unique_ptr<Enemy>> enemies_ = {};

};

