#include "NormalBullet.h"
#include "MapCheck.h"

void NormalBullet::Initialize(GameContext* context) {
	context_ = context;
	particle_ = std::make_unique<ParticleSystem>();
	particle_->Initialize(context->LoadInstancedModel("Resources/Particle/Fire", "fireEffect.obj", particleNum_));
	particle_->SetLifeTime(2);
	particle_->SetColor({ 0.5f, 0.7f, 0.0f, 1.0f });

	hitParticle_ = std::make_unique<ParticleSystem>();
	hitParticle_->Initialize(context->LoadInstancedModel("Resources/Particle/Fire", "fireEffect.obj", hitParticleNum_));
	hitParticle_->SetLifeTime(hitParticleLifeTime);
	hitParticle_->SetColor({ 0.5f, 0.7f, 0.0f, 1.0f });
	particleField_ = std::make_unique<ParticleField>();
	particleField_->SetCheckArea(false);
}

void NormalBullet::Update(MapCheck* mapCheck) {
	if (!isDead_) {
		prePos_ = model_->GetTransform().translate;
		model_->SetTranslate(model_->GetTransform().translate + velocity_);

		// マップ当たり判定
		Vector2 pos = { model_->GetTransform().translate.x,model_->GetTransform().translate.z };

		lifeTime_--;
		if (lifeTime_ <= 0) {
			Hit();
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
			particle_->Emit(transform, -velocity_ * 0.5f);
		}
	}
	particle_->Update();

	if (isDead_) {
		hitParticle_->Update();
		hitParticleLifeTime--;
		if (hitParticleLifeTime <= 0) {
			canErase_ = true;
		}
	}
}

void NormalBullet::Draw(GameContext* context, Camera* camera) {
	if (!isDead_) {
		context->DrawModel(model_.get(), camera, BlendMode::Add);
	}

	// パーティクル
	context->DrawParticle(particle_.get(), camera, BlendMode::Add);
	context->DrawParticle(hitParticle_.get(), camera, BlendMode::Add);
}

void NormalBullet::Hit() {
	isDead_ = true;

	// 飛散パーティクル
	particleField_->SetGravity(-0.4f, model_->GetTransform().translate);
	hitParticle_->AddField(std::move(particleField_));
	for (int i = 0; i < hitParticleNum_; ++i) {
		Vector3 randomVector = {
		context_->RandomFloat(-particleRange_ / 2.0f, particleRange_ / 2.0f),
		context_->RandomFloat(-particleRange_ / 2.0f, particleRange_ / 2.0f),
		context_->RandomFloat(-particleRange_ / 2.0f, particleRange_ / 2.0f),
		};
		Transform transform = model_->GetTransform();
		transform.translate += randomVector;
		transform.scale = { 1.5f,1.5f,1.5f };
		hitParticle_->Emit(transform, {});
	}
}