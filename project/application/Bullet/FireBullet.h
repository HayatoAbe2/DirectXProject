#pragma once
#include "Bullet.h"
#include "ParticleField.h"
#include "GameContext.h"
class FireBullet : public Bullet{
public:
	FireBullet(std::unique_ptr<Model> model, const Vector3& direction, const RangedWeaponStatus& status, bool isEnemyBullet) :
		Bullet(std::move(model), direction, status, isEnemyBullet) {};
	void Initialize(GameContext* context);
	void Update(MapCheck* mapCheck) override;
	void Draw(GameContext* context, Camera* camera) override;
	void Hit() override;

private:
	GameContext* context_ = nullptr;
	
	std::unique_ptr<ParticleSystem> particle_;
	const int particleNum_ = 200;
	float particleRange_ = 1.5f;

	std::unique_ptr<ParticleSystem> explosionParticle_;
	const int explosionParticleNum_ = 250;
	float explosionParticleRange_ = 1.0f;
	int explosionEndLifeTime = 8;

	std::unique_ptr<ParticleField> particleField_;

	// ライトのインデックス
	int lightIndex_ = 0;

	// カメラ揺らし
	bool shaked_ = false;
};

