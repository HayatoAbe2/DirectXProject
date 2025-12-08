#include "Bat.h"

void Bat::Attack(RangedWeapon* rangedWeapon, BulletManager* bulletManager, GameContext* context) {
	if (attackCoolTimer_ <= 0) {
		// 射撃
		attackCoolTimer_ = rangedWeapon_->Shoot(model_->GetTransform().translate, attackDirection_, bulletManager, context, true);

	} else {
		attackCoolTimer_--;
	}
}