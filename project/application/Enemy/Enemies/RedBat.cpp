#include "RedBat.h"

void RedBat::Attack(Weapon* weapon, BulletManager* bulletManager, GameContext* context, Camera* camera) {
	// 連続攻撃開始
	if (attackCoolTimer_ <= 0) {
		comboCount_ = 0;
	}

	// 攻撃中
	if (comboCount_ < maxCombo_) {
		comboInterval_--;

		if (comboInterval_ <= 0) {
			// 射撃
			attackCoolTimer_ += weapon_->Shoot(model_->GetTransform().translate, attackDirection_, bulletManager, context, camera, true);

			comboCount_++;
			comboInterval_ = maxComboInterval_;
		}
	} else {
		// 終了後
		attackCoolTimer_ -= 5;

		// 武器を交換する
		bossWeapons_[weaponNum_] = std::move(weapon_);
		if (bossWeapons_.size() - 1 == weaponNum_) {
			// 0番目に戻る
			weapon_ = std::move(bossWeapons_[0]);
			weaponNum_ = 0;
		} else {
			// 次の武器
			weapon_ = std::move(bossWeapons_[++weaponNum_]);
		}
	}
}