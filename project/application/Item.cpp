#include "Item.h"
#include "GameContext.h"
#include "Camera.h"
#include <numbers>

Item::Item(std::unique_ptr<RangedWeapon> rangedWeapon, Vector3 pos) {
	rangedWeapon_ = std::move(rangedWeapon);
	rangedWeapon_->GetWeaponModel()->SetTranslate(pos);
	rangedWeapon_->GetWeaponModel()->SetRotate({ float(std::numbers::pi / 2.0f),0,float(std::numbers::pi / 2.0f) });
}

void Item::Draw(GameContext* context, Camera* camera) {
	context->DrawEntity(*rangedWeapon_->GetWeaponModel(), *camera);
}
