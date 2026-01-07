#include "Item.h"
#include "GameContext.h"
#include "Camera.h"
#include <numbers>
#include "FireBall.h"

Item::~Item() {
	context_->RemovePointLight(lightIndex_);
}

Item::Item(std::unique_ptr<RangedWeapon> rangedWeapon, Vector3 pos,GameContext* context,Rarity rarity) {
	rangedWeapon_ = std::move(rangedWeapon);
	rangedWeapon_->GetWeaponModel()->SetTranslate(pos);
	if (rangedWeapon_ && dynamic_cast<FireBall*>(rangedWeapon_.get())) {
	} else {
		rangedWeapon_->GetWeaponModel()->SetRotate({ 0,float(std::numbers::pi / 2.0f),0 });
	}

	lightIndex_ = context->AddPointLight();
	auto& pointLight = context->GetPointLight(lightIndex_);
	pointLight.position = pos;
	pointLight.intensity = 1.0f;
	pointLight.radius = 3.0f;
	switch (rarity) {
	case static_cast<int>(Rarity::Common):
		pointLight.color = { 0.5f,0.5f,0.5f,1.0f };
		break;
	case static_cast<int>(Rarity::Rare):
		pointLight.color = { 0.1f,0.1f,0.7f,1.0f };
		break;
	case static_cast<int>(Rarity::Epic):
		pointLight.color = { 0.8f,0.1f,0.8f,1.0f };
		break;
	case static_cast<int>(Rarity::Legendary):
		pointLight.color = { 1.0f,0.8f,0.0f,1.0f };
		break;
	}

	context_ = context;
}

void Item::Draw(GameContext* context, Camera* camera) {
	context->DrawModel(rangedWeapon_->GetWeaponModel(), camera);
}

void Item::Erase() {
	// 落ちているアイテムの削除
	isDead_ = true;

	// ライト削除
	context_->RemovePointLight(lightIndex_);
}
