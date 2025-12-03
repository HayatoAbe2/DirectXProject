#include "RapidBullet.h"
#include "GameContext.h"
#include "MapCheck.h"

void RapidBullet::Initialize(GameContext* context) {
	context_ = context;
	particle_ = std::make_unique<Entity>();
	particle_->SetParticleSystem(context->LoadInstancedModel("Resources/Particle/Fire", "fireEffect.obj", particleNum_));
	particle_->GetParticleSystem()->SetLifeTime(2);
	particle_->GetParticleSystem()->SetColor({ 0.0f, 0.1f, 0.8f, 1.0f });

	hitParticle_ = std::make_unique<Entity>();
	hitParticle_->SetParticleSystem(context->LoadInstancedModel("Resources/Particle/Fire", "fireEffect.obj", hitParticleNum_));
	hitParticle_->GetParticleSystem()->SetLifeTime(hitParticleLifeTime);
	hitParticle_->GetParticleSystem()->SetColor({ 0.2f, 0.1f, 1.0f, 1.0f });
	particleField_ = std::make_unique<ParticleField>();
	particleField_->SetCheckArea(false);
}

void RapidBullet::Update(MapCheck* mapCheck) {
	if (!isDead_) {
		prePos_ = model_->GetTransform().translate;
		model_->SetTranslate(model_->GetTransform().translate + velocity_);

		// マップ当たり判定
		Vector2 pos = { model_->GetTransform().translate.x,model_->GetTransform().translate.z };

		lifeTime_--;
		if (lifeTime_ <= 0) {
			canErase_ = true;
		}

		if (mapCheck->IsHitWall(pos, status_.bulletSize / 2.0f)) {
			Hit();
		}

		// パーティクル
		for (int i = 0; i < 50; ++i) {
			Vector3 randomVector = {
			context_->RandomFloat(-particleRange_ / 2.0f, particleRange_ / 2.0f),
			context_->RandomFloat(-particleRange_ / 2.0f, particleRange_ / 2.0f),
			context_->RandomFloat(-particleRange_ / 2.0f, particleRange_ / 2.0f),
			};
			Transform transform = model_->GetTransform();
			transform.translate += randomVector + velocity_ * 0.5f;
			transform.scale = { 1.0f,1.0f,1.0f };
			particle_->GetParticleSystem()->Emit(transform, -velocity_ * 0.5f);
		}
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

void RapidBullet::Draw(GameContext* context, Camera* camera) {
	if (!isDead_) {
		//context->DrawEntity(*model_, *camera, BlendMode::Add);
	}

	// パーティクル
	context->DrawEntity(*particle_, *camera, BlendMode::Add);
	context->DrawEntity(*hitParticle_, *camera, BlendMode::Add);
}

void RapidBullet::Hit() {
	isDead_ = true;

	// 飛散パーティクル
	particleField_->SetGravity(-0.4f, model_->GetTransform().translate);
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