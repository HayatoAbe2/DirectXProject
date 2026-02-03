#pragma once
#include "Weapon.h"
#include "MathUtils.h"
#include <vector>
#include <memory>

class BulletManager;

class FireBall : public Weapon {
public:
	FireBall(const WeaponStatus& status, std::unique_ptr<Model> model, std::unique_ptr<Model> shadowModel, GameContext* context);
	int Shoot(Vector3 pos, Vector3 dir, BulletManager* bulletManager, GameContext* context, Camera* camera, bool isEnemyBullet) override;
	void Update() override;

private:
};

