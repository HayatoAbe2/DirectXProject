#include "FireBullet.h"
#include "GameContext.h"
#include "MapCheck.h"
#include "ParticleSystem.h"
#include "Camera.h"

void FireBullet::Initialize(GameContext* context) {
	context_ = context;
	particle_ = std::make_unique<Entity>();
	particle_->SetParticleSystem(context->LoadInstancedModel("Resources/Particle/Fire", "fireEffect.obj", particleNum_));
	particle_->GetParticleSystem()->SetLifeTime(10);
	particle_->GetParticleSystem()->SetColor({ 1.0f, 0.03f, 0.0f, 1.0f });

	lightIndex_ = context_->AddPointLight();
	auto& light = context_->GetPointLight(lightIndex_);
	light.radius = 1.0f;
}

void FireBullet::Update(MapCheck* mapCheck) {
	model_->SetTranslate(model_->GetTransform().translate + velocity_);

	// マップ当たり判定
	Vector2 pos = { model_->GetTransform().translate.x,model_->GetTransform().translate.z };

	maxLifeTime_--;
	if (maxLifeTime_ <= 0 || mapCheck->IsHitWall(pos, status_.bulletSize / 2.0f)) {
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

	if (!isDead_) {
		auto& light = context_->GetPointLight(lightIndex_);
		light.position = model_->GetTransform().translate;
	}
}

void FireBullet::Draw(GameContext* context, Camera* camera) {
	if (isDead_) {
		camera->StartShake(1.0f, 3);
	}
	context->DrawEntity(*model_, *camera);
	context->DrawEntity(*particle_, *camera,BlendMode::Add);
}

void FireBullet::Hit() {
	isDead_ = true;
	context_->RemovePointLight(lightIndex_);
}