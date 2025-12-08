#include "Knight.h"

void Knight::Attack(RangedWeapon* rangedWeapon, BulletManager* bulletManager, GameContext* context) {
	// 連続攻撃開始
	if (attackCoolTimer_ <= 0) {
		comboCount_ = 0;
	}
	
	// 攻撃中
	if (comboCount_ < maxCombo_) {
		comboInterval_--;
		
		if (comboInterval_ <= 0) {
			// 射撃
			attackCoolTimer_ += rangedWeapon_->Shoot(model_->GetTransform().translate, attackDirection_, bulletManager, context, true);

			comboCount_++;
			comboInterval_ = maxComboInterval_;
		}
	} else {
		// 終了後
		attackCoolTimer_--;
	}
}