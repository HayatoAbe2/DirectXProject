#pragma once
#include "MathUtils.h"
#include "Camera.h"
#include "Enemy.h"
#include <vector>
#include <memory>

class GameContext;
class MapCheck;
class Player;
class Bullet;

class EnemyManager {
public:
	void Initialize(GameContext* context);
	void Update(GameContext* context, MapCheck* mapCheck, Player* player, std::vector<std::unique_ptr<Bullet>>& bullets);
	void Draw(GameContext* context, Camera* camera);
	void Spawn(Vector3 pos,GameContext* context);
private:
	std::vector<std::unique_ptr<Enemy>> enemies_ = {};

};

