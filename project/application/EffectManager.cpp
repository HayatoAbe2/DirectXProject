#include "EffectManager.h"
#include "GameContext.h"
#include "Camera.h"
#include "Sprite.h"

void EffectManager::Initialize(GameContext* context) {
	context_ = context;
}

void EffectManager::Update() {
	for (auto& effect : hitEffect_) {
		// 拡大
		effect->GetSprite()->SetSize(effect->GetSprite()->GetSize() + Vector2{ hitEffectUpScaleSpeed_, hitEffectUpScaleSpeed_ });
		
		// 薄くする
		effect->SetColor({ 1.0f,1.0f,1.0f,effect->GetColor().z - 0.02f });
	}

	// 消滅したエフェクトの削除
	hitEffect_.erase(
		std::remove_if(hitEffect_.begin(), hitEffect_.end(),
			[](const std::unique_ptr<Entity>& effect) {
				return effect->GetColor().z <= 0.0f;
			}
		),
		hitEffect_.end()
	);
}

void EffectManager::Draw(GameContext* context,Camera* camera) {
	for (const auto& effect : hitEffect_) {
		context->DrawEntity(*effect, *camera);
	}
}

void EffectManager::SpawnHitEffect(const Vector3& pos) {
	auto effect = std::make_unique<Entity>();
	effect->SetTranslate(pos);
	effect->SetSprite(context_->LoadSprite("Resources/Effects/hitEffect.png"));
	hitEffect_.push_back(std::move(effect));
}
