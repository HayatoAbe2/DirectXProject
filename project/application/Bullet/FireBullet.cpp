#include "FireBullet.h"
#include "GameContext.h"
#include "MapCheck.h"
#include "ParticleSystem.h"
#include "Camera.h"
#include <sstream>

FireBullet::~FireBullet() {
}

void FireBullet::Initialize(GameContext* context) {
	context_ = context;
	particle_ = std::make_unique<Entity>();
	particle_->SetParticleSystem(context->LoadInstancedModel("Resources/Particle/Fire", "fireEffect.obj", particleNum_));
	particle_->GetParticleSystem()->SetLifeTime(10);
	particle_->GetParticleSystem()->SetColor({ 0.3f, 0.03f, 0.0f, 1.0f });
 
	std::ostringstream oss;
	oss << "particle model ptr=" << particle_->GetParticleSystem()->GetInstancedModel_().get() << " use_count=" << particle_->GetParticleSystem()->GetInstancedModel_().use_count() << "\n";
	OutputDebugStringA(oss.str().c_str());

	explosionParticle_ = std::make_unique<Entity>();
	explosionParticle_->SetParticleSystem(context->LoadInstancedModel("Resources/Particle/Fire", "fireEffect.obj", particleNum_));
	explosionParticle_->GetParticleSystem()->SetLifeTime(10);
	explosionParticle_->GetParticleSystem()->SetColor({ 0.7f, 0.03f, 0.0f, 1.0f });
	particleField_ = std::make_unique<ParticleField>();
	particleField_->SetCheckArea(false);

	lightIndex_ = context_->AddPointLight();
	auto& light = context_->GetPointLight(lightIndex_);
	light.radius = 1.0f;
}

void FireBullet::Update(MapCheck* mapCheck) {
	if (!isDead_) {
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
			transform.scale = { 2.0f,2.0f,2.0f };
			particle_->GetParticleSystem()->Emit(transform, -velocity_ * 0.2f);
		}
	}
	particle_->GetParticleSystem()->Update();

	if (!isDead_) {
		auto& light = context_->GetPointLight(lightIndex_);
		light.position = model_->GetTransform().translate;
	} else {
		explosionParticle_->GetParticleSystem()->Update();
		explosionEndLifeTime--;
		if (explosionEndLifeTime <= 0) {
			canErase_ = true;
		}
	}
}

void FireBullet::Draw(GameContext* context, Camera* camera) {
	if (isDead_ && !shaked_) {
		camera->StartShake(1.0f, 3);
		shaked_ = true;
	}

	if (!isDead_) {
		context->DrawEntity(*model_, *camera,BlendMode::Add);
	}

	// パーティクル
	context->DrawEntity(*particle_, *camera, BlendMode::Add);
	context->DrawEntity(*explosionParticle_, *camera, BlendMode::Add);
}

void FireBullet::Hit() {
	isDead_ = true;
	context_->RemovePointLight(lightIndex_);

	// 爆発開始
	particleField_->SetGravity(-0.6f, model_->GetTransform().translate);
	explosionParticle_->GetParticleSystem()->AddField(std::move(particleField_));
	for (int i = 0; i < explosionParticleNum_; ++i) {
		Vector3 randomVector = {
		context_->RandomFloat(-particleRange_ / 2.0f, particleRange_ / 2.0f),
		context_->RandomFloat(-particleRange_ / 2.0f, particleRange_ / 2.0f),
		context_->RandomFloat(-particleRange_ / 2.0f, particleRange_ / 2.0f),
		};
		Transform transform = model_->GetTransform();
		transform.translate += randomVector;
		transform.scale = { 1.5f,1.5f,1.5f };
		explosionParticle_->GetParticleSystem()->Emit(transform, {});
	}
}