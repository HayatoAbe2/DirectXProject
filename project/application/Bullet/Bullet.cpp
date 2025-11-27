#include "Bullet.h"
#include "GameContext.h"
#include "Camera.h"
#include "App.h"
#include "MapCheck.h"
#include <numbers>

Bullet::Bullet(std::unique_ptr<Entity> model, const Vector3& direction, const RangedWeaponStatus& status,bool isEnemyBullet) {
	model_ = std::move(model);
	velocity_ = direction * status.bulletSpeed;
	if (isEnemyBullet) { velocity_ / 2.0f; }
	status_ = status;
	maxLifeTime_ = status.bulletLifeTime;
	isEnemyBullet_ = isEnemyBullet;
	model_->SetScale({ status.bulletSize,status.bulletSize ,status.bulletSize });
	model_->SetRotate({ 0, -std::atan2(velocity_.z, velocity_.x) + float(std::numbers::pi) / 2.0f,0 });
}

void Bullet::Update(MapCheck* mapCheck) {
	model_->SetTranslate(model_->GetTransform().translate + velocity_);
	
	// マップ当たり判定
	Vector2 pos = { model_->GetTransform().translate.x,model_->GetTransform().translate.z };

	maxLifeTime_--;
	if (maxLifeTime_ <= 0 || mapCheck->IsHitWall(pos,status_.bulletSize / 2.0f)) {
		Hit();
	}
}

void Bullet::Draw(GameContext* context,Camera* camera) {
	context->DrawEntity(*model_,*camera);
}

void Bullet::Hit() {
	isDead_ = true;
}
