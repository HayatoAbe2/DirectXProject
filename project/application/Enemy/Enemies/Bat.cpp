#include "Bat.h"

void Bat::Attack(Weapon* weapon, BulletManager* bulletManager, GameContext* context, Camera* camera) {
	if (attackCoolTimer_ <= 0) {
		// 射撃
		attackCoolTimer_ = weapon_->Shoot(model_->GetTransform().translate, attackDirection_, bulletManager, context, camera, true);

	} else {
		attackCoolTimer_--;
	}
}