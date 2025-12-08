#include "HeavyKnight.h"

void HeavyKnight::Attack(RangedWeapon* rangedWeapon, BulletManager* bulletManager, GameContext* context) {
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
		attackCoolTimer_ -= 5;

		// 武器を交換する
		bossWeapons_[weaponNum_] = std::move(rangedWeapon_);
		if (bossWeapons_.size() - 1 == weaponNum_) {
			// 0番目に戻る
			rangedWeapon_ = std::move(bossWeapons_[0]);
			weaponNum_ = 0;
		} else {
			// 次の武器
			rangedWeapon_ = std::move(bossWeapons_[++weaponNum_]);
		}
	}
}