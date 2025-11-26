#pragma once
#include "Bullet.h"
class FireBullet : public Bullet{
public:
	FireBullet(std::unique_ptr<Entity> model, const Vector3& direction, const RangedWeaponStatus& status, bool isEnemyBullet) :
		Bullet(std::move(model), direction, status, isEnemyBullet) {};
	void Initialize(GameContext* context);
	void Update(MapCheck* mapCheck) override;
	void Draw(GameContext* context, Camera* camera) override;
	void Hit() override;

private:
	GameContext* context_ = nullptr;
	std::unique_ptr<Entity> particle_;
	const int particleNum_ = 200;
	float particleRange_ = 1.0f;

	// ライトのインデックス
	int lightIndex_ = 0;
};

