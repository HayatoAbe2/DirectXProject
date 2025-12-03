#include "Bullet.h"
#include "GameContext.h"
#include "Camera.h"
#include "App.h"
#include "MapCheck.h"
#include <numbers>

Bullet::Bullet(std::unique_ptr<Entity> model, const Vector3& direction, const RangedWeaponStatus& status,bool isEnemyBullet) {
	model_ = std::move(model);
	velocity_ = direction * status.bulletSpeed;
	if (isEnemyBullet) { velocity_ /= 2.0f; }
	status_ = status;
	lifeTime_ = status.bulletLifeTime;
	isEnemyBullet_ = isEnemyBullet;
	model_->SetScale({ status.bulletSize,status.bulletSize ,status.bulletSize });
	model_->SetRotate({ 0, -std::atan2(velocity_.z, velocity_.x) + float(std::numbers::pi) / 2.0f,0 });
}