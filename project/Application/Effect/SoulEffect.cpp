#include "SoulEffect.h"

void SoulEffect::Initialize(GameContext* context, Vector3 pos, Vector3 goal) {
	context_ = context;
	pos_ = pos;
	goal_ = goal;

	particle_ = std::make_unique<ParticleSystem>();
	particle_->Initialize(context->LoadInstancedModel("Resources/Particle/Fire", "fireEffect.obj", particleNum_));
	particle_->SetLifeTime(10);
	particle_->SetColor({ 0.8f, 0.0f, 0.8f, 1.0f });
}

void SoulEffect::Update() {
	// パーティクル
	for (int i = 0; i < 20; ++i) {
		Vector3 randomVector = {
		context_->RandomFloat(-particleRange_ / 2.0f, particleRange_ / 2.0f),
		context_->RandomFloat(-particleRange_ / 2.0f, particleRange_ / 2.0f),
		context_->RandomFloat(-particleRange_ / 2.0f, particleRange_ / 2.0f),
		};
		Transform transform;
		transform.translate += randomVector;
		particle_->Emit(transform, {});
	}
}

void SoulEffect::Draw(Camera* camera) {
	context_->DrawParticle(particle_.get(), camera, BlendMode::Add);
}