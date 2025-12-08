#pragma once
#include "Bullet.h"
#include "ParticleSystem.h"

class RapidBullet :public Bullet {
public:
	RapidBullet(std::unique_ptr<Entity> model, const Vector3& direction, const RangedWeaponStatus& status, bool isEnemyBullet) :
		Bullet(std::move(model), direction, status, isEnemyBullet) {
	};
	void Initialize(GameContext* context);
	void Update(MapCheck* mapCheckz) override;
	void Draw(GameContext* context, Camera* camera) override;
	void Hit() override;

private:
	GameContext* context_ = nullptr;

	std::unique_ptr<Entity> particle_;
	const int particleNum_ = 150;
	float particleRange_ = 0.7f;

	std::unique_ptr<Entity> hitParticle_;
	const int hitParticleNum_ = 10;
	float hitParticleRange_ = 1.0f;
	int hitParticleLifeTime = 20;

	std::unique_ptr<ParticleField> particleField_;
};
