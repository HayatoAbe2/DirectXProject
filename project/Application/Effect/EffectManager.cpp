 #include "EffectManager.h"
#include "GameContext.h"
#include "Camera.h"
#include "Sprite.h"
#include <numbers>

void EffectManager::Initialize(GameContext* context) {
	context_ = context;
}

void EffectManager::Update() {
	for (auto& effect : hitEffect_) {
		// 拡大
		effect->SetScale(effect->GetTransform().scale + Vector3{ hitEffectUpScaleSpeed_, hitEffectUpScaleSpeed_, hitEffectUpScaleSpeed_});
		
		// 薄くする
		MaterialData data = effect->GetData()->defaultMaterials_[0]->GetData();
		data.color.w -= 0.1f;
		effect->GetData()->defaultMaterials_[0]->SetData(data);
	}

	// 消滅したエフェクトの削除
	hitEffect_.erase(
		std::remove_if(hitEffect_.begin(), hitEffect_.end(),
			[](const std::unique_ptr<Model>& effect) {
				return effect->GetData()->defaultMaterials_[0]->GetData().color.w <= 0.0f;
			}
		),
		hitEffect_.end()
	);
}

void EffectManager::Draw(GameContext* context,Camera* camera) {
	for (const auto& effect : hitEffect_) {
		Vector3 rotate = camera->transform_.rotate;
		rotate.x += float(std::numbers::pi);
		effect->SetRotate(rotate);
		context->DrawModel(effect.get(), camera, BlendMode::Add);
	}
}

void EffectManager::SpawnHitEffect(const Vector3& pos) {
	auto effect = std::make_unique<Model>();
	effect = context_->LoadModel("Resources/HitEffect", "hitEffect.obj");
	effect->SetTransform({ {0.5f,0.5f,0.5f},{},pos });
	hitEffect_.push_back(std::move(effect));
}

void EffectManager::SpawnSoulEffect(const Vector3& spawnPos, const Vector3& goalPos) {

}
