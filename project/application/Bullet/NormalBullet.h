#pragma once
#include "Bullet.h"
class NormalBullet :public Bullet{
public:
	NormalBullet(std::unique_ptr<Entity> model, const Vector3& direction, const RangedWeaponStatus& status, bool isEnemyBullet) :
		Bullet(std::move(model), direction, status, isEnemyBullet) {
	};
	void Update(MapCheck* mapCheck) override;
	void Draw(GameContext* context, Camera* camera) override;
	void Hit() override;
};

