#include "Bullet.h"
#include "GameContext.h"
#include "Entity.h"
#include "Camera.h"
#include "App.h"
#include <numbers>

Bullet::Bullet(std::unique_ptr<Entity> model, const Vector3& velocity, const RangedWeaponStatus& status) {
	model_ = std::move(model);
	velocity_ = velocity;
	status_ = status;
	model_->SetScale({ status.bulletRadius,status.bulletRadius ,status.bulletRadius });
	model_->SetRotate({ 0, -std::atan2(velocity_.z, velocity_.x) + float(std::numbers::pi) / 2.0f,0 });
}

void Bullet::Update() {
	model_->SetTranslate(model_->GetTransform().translate + velocity_);
}

void Bullet::Draw(GameContext* context,Camera* camera) {
	context->DrawEntity(*model_,*camera);
}
