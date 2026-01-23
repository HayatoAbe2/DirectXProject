#pragma once
#include "Bullet.h"
#include "ParticleSystem.h"
#include "GameContext.h"

class SpreadBullet :public Bullet {
public:
	SpreadBullet(std::unique_ptr<Model> model, const Vector3& direction, const WeaponStatus& status, bool isEnemyBullet) :
		Bullet(std::move(model), direction, status, isEnemyBullet) {
	};
	void Initialize(GameContext* context);
	void Update(MapCheck* mapCheck) override;
	void Draw(GameContext* context, Camera* camera) override;
	void Hit() override;

private:
	GameContext* context_ = nullptr;

	std::unique_ptr<ParticleSystem> particle_;
	const int particleNum_ = 150;
	float particleRange_ = 0.7f;

	std::unique_ptr<ParticleSystem> hitParticle_;
	const int hitParticleNum_ = 10;
	float hitParticleRange_ = 1.0f;
	int hitParticleLifeTime = 20;

	std::unique_ptr<ParticleField> particleField_;
};

