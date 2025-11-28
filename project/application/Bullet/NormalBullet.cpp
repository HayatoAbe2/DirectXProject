#include "NormalBullet.h"
#include "GameContext.h"
#include "MapCheck.h"

void NormalBullet::Initialize(GameContext* context) {
	context_ = context;
	particle_ = std::make_unique<Entity>();
	particle_->SetParticleSystem(context->LoadInstancedModel("Resources/Particle/Fire", "fireEffect.obj", particleNum_));
	particle_->GetParticleSystem()->SetLifeTime(7);
	particle_->GetParticleSystem()->SetColor({ 0.2f, 0.03f, 0.0f, 1.0f });

	hitParticle_ = std::make_unique<Entity>();
	hitParticle_->SetParticleSystem(context->LoadInstancedModel("Resources/Particle/Fire", "fireEffect.obj", particleNum_));
	hitParticle_->GetParticleSystem()->SetLifeTime(hitParticleLifeTime);
	hitParticle_->GetParticleSystem()->SetColor({ 0.5f, 0.7f, 0.0f, 1.0f });
	particleField_ = std::make_unique<ParticleField>();
	particleField_->SetUseArea(false);
}

void NormalBullet::Update(MapCheck* mapCheck) {
	model_->SetTranslate(model_->GetTransform().translate + velocity_);

	// マップ当たり判定
	Vector2 pos = { model_->GetTransform().translate.x,model_->GetTransform().translate.z };

	maxLifeTime_--;
	if (maxLifeTime_ <= 0) {
		canErase_ = true;
	}

	if (mapCheck->IsHitWall(pos, status_.bulletSize / 2.0f)) {
		Hit();
	}

	// パーティクル
	for (int i = 0; i < 20; ++i) {
		Vector3 randomVector = {
		context_->RandomFloat(-particleRange_ / 2.0f, particleRange_ / 2.0f),
		context_->RandomFloat(-particleRange_ / 2.0f, particleRange_ / 2.0f),
		context_->RandomFloat(-particleRange_ / 2.0f, particleRange_ / 2.0f),
		};
		Transform transform = model_->GetTransform();
		transform.translate += randomVector + velocity_ * 0.5f;
		transform.scale = { 1.5f,1.5f,1.5f };
		particle_->GetParticleSystem()->Emit(transform, -velocity_ * 0.2f);
	}
	particle_->GetParticleSystem()->Update();

	if (isDead_) {
		hitParticle_->GetParticleSystem()->Update();
		hitParticleLifeTime--;
		if (hitParticleLifeTime <= 0) {
			canErase_ = true;
		}
	}
}

void NormalBullet::Draw(GameContext* context, Camera* camera) {
	context->DrawEntity(*model_, *camera, BlendMode::Add);

	// パーティクル
	context->DrawEntity(*particle_, *camera, BlendMode::Add);
	context->DrawEntity(*hitParticle_, *camera, BlendMode::Add);
}

void NormalBullet::Hit() {
	isDead_ = true;
	canErase_ = true;

	// 飛散パーティクル
	particleField_->SetGravity(-0.5f, model_->GetTransform().translate);
	hitParticle_->GetParticleSystem()->AddField(std::move(particleField_));
	for (int i = 0; i < hitParticleNum_; ++i) {
		Vector3 randomVector = {
		context_->RandomFloat(-particleRange_ / 2.0f, particleRange_ / 2.0f),
		context_->RandomFloat(-particleRange_ / 2.0f, particleRange_ / 2.0f),
		context_->RandomFloat(-particleRange_ / 2.0f, particleRange_ / 2.0f),
		};
		Transform transform = model_->GetTransform();
		transform.translate += randomVector;
		transform.scale = { 1.5f,1.5f,1.5f };
		hitParticle_->GetParticleSystem()->Emit(transform, {});
	}
}