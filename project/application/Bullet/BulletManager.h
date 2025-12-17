#pragma once
#include "Bullet.h"
#include "MathUtils.h"
#include "WeaponStatus.h"
#include <vector>
#include <memory>

class MapCheck;
class EffectManager;
class GameContext;
class Camera;

class BulletManager {
public:
	void Update(MapCheck* mapCheck);
	void Draw(GameContext* context, Camera* camera);
	void Reset();

	void AddBullet(std::unique_ptr<Bullet> newBullet);
	std::vector<Bullet*> GetBullets();
private:
	// 弾
	std::vector<std::unique_ptr<Bullet>> bullets_;
};

