#include "Item.h"
#include "GameContext.h"
#include "Camera.h"
#include <numbers>

Item::~Item() {
	context_->RemovePointLight(lightIndex_);
}

Item::Item(std::unique_ptr<RangedWeapon> rangedWeapon, Vector3 pos,GameContext* context) {
	rangedWeapon_ = std::move(rangedWeapon);
	rangedWeapon_->GetWeaponModel()->SetTranslate(pos);
	rangedWeapon_->GetWeaponModel()->SetRotate({ 0,float(std::numbers::pi / 2.0f),0 });

	lightIndex_ = context->AddPointLight();
	auto& pointLight = context->GetPointLight(lightIndex_);
	pointLight.position = pos;
	pointLight.intensity = 1.0f;
	pointLight.radius = 3.0f;

	context_ = context;
}

void Item::Draw(GameContext* context, Camera* camera) {
	context->DrawEntity(*rangedWeapon_->GetWeaponModel(), *camera);
}

void Item::Erase() {
	// 落ちているアイテムの削除
	isDead_ = true;

	// ライト削除
	context_->RemovePointLight(lightIndex_);
}
