#include "Bullet.h"
#include "GameContext.h"
#include "Camera.h"
#include "App.h"
#include "MapCheck.h"
#include <numbers>

Bullet::Bullet(std::unique_ptr<Model> model, const Vector3& direction, const WeaponStatus& status, bool isEnemyBullet) {
	model_ = std::move(model);
	status_ = status;

	// 追加効果
	for (auto enchant : status.enchants) {
		switch (enchant) {
		case static_cast<int>(Enchants::increaseDamage):
			status_.damage *= 1.3f;
			break;
		case static_cast<int>(Enchants::bigBullet):
			status_.bulletSize += 0.3f;
			break;
		case static_cast<int>(Enchants::fastBullet):
			status_.bulletSpeed *= 1.5f;
			break;
		case static_cast<int>(Enchants::hardKnockback):
			status_.knockback *= 1.5f;
			break;
		}
	}

	velocity_ = direction * status.bulletSpeed;
	if (isEnemyBullet) { velocity_ /= 2.0f; }
	lifeTime_ = status.bulletLifeTime;
	isEnemyBullet_ = isEnemyBullet;
	model_->SetScale({ status.bulletSize,status.bulletSize ,status.bulletSize });
	model_->SetRotate({ 0, -std::atan2(velocity_.z, velocity_.x) + float(std::numbers::pi) / 2.0f,0 });
}