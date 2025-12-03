#include "Enemy.h"
#include "Entity.h"
#include "GameContext.h"
#include "Camera.h"
#include "BulletManager.h"
#include "MapCheck.h"
#include "Player.h"
#include "Mesh.h"
#include "EnemyStatus.h"

#include <numbers>
#include <cmath>

Enemy::Enemy(std::unique_ptr<Entity> model, Vector3 pos, EnemyStatus status, std::unique_ptr<RangedWeapon> rWeapon) {
	model_ = std::move(model);
	model_->SetTranslate(pos);
	status_ = status;

	rangedWeapon_ = std::move(rWeapon);
}

Enemy::Enemy(std::unique_ptr<Entity> model, Vector3 pos, EnemyStatus status, std::vector<std::unique_ptr<RangedWeapon>> rWeapons) {
	model_ = std::move(model);
	model_->SetTranslate(pos);
	status_ = status;

	multipleWeapons_ = std::move(rWeapons);
	rangedWeapon_ = std::move(multipleWeapons_[0]);
	isBoss_ = true;
	overheat_ = 8;
}

void Enemy::Update(GameContext* context, MapCheck* mapCheck, Player* player, BulletManager* bulletManager) {
	for (auto& mesh : model_->GetModel()->GetMeshes()) {
		auto data = mesh->GetMaterial()->GetData();
		data.color = { 1.0f,1.0f,1.0f,1.0f };
		mesh->GetMaterial()->SetData(data);
	}

	if (!isFall_) {

		if (stunTimer_ <= 0) {
			// 移動
			if (target_) {
				if (isMoving_) {
					moveTimer_++;
					if (moveTimer_ > status_.moveTime) {
						moveTimer_ = 0;
						isMoving_ = false;
						velocity_ = {};
					}
				} else {
					stopTimer_++;
					if (stopTimer_ > status_.stopTime) {
						stopTimer_ = 0;
						isMoving_ = true;

						Vector2 direction = Normalize(Vector2{ context->RandomFloat(-1,1),context->RandomFloat(-1,1) });
						velocity_.x = direction.x * status_.moveSpeed;
						velocity_.z = direction.y * status_.moveSpeed;
					}
				}
			} else {
				moveTimer_ = 0;
				stopTimer_ = 0;
				velocity_ = { 0,0,0 };
			}

			// 速度をもとに移動
			Vector2 pos = { model_->GetTransform().translate.x,model_->GetTransform().translate.z };
			pos.x += velocity_.x;
			mapCheck->ResolveCollisionX(pos, status_.radius, status_.canFly);
			pos.y += velocity_.z;
			mapCheck->ResolveCollisionY(pos, status_.radius, status_.canFly);
			model_->SetTranslate({ pos.x,model_->GetTransform().translate.y,pos.y });

			if (velocity_.x != 0 || velocity_.z != 0) {
				model_->SetRotate({ 0,-std::atan2(velocity_.z, velocity_.x) + float(std::numbers::pi) / 2.0f,0 });
			}

			// ターゲット発見
			if (Length(player->GetTransform().translate - model_->GetTransform().translate) < searchRadius_) {
				target_ = player;
				loseSightTimer_ = 0;
			} else {
				// 見失う
				loseSightTimer_++;
				if (loseSightTimer_ > status_.loseSightTime) {
					target_ = nullptr;
				}
			}

			if (target_) {
				// 攻撃の向き
				attackDirection_ = Normalize(target_->GetTransform().translate - model_->GetTransform().translate);
				model_->SetRotate({ 0,-std::atan2(attackDirection_.z, attackDirection_.x) + float(std::numbers::pi) / 2.0f,0 });

				if (status_.loseSightRadius < Length(target_->GetTransform().translate - model_->GetTransform().translate)) {
					target_ = nullptr;
					searchRadius_ = status_.defaultSearchRadius;
				}


				if (attackCoolTimer_ <= 0) {
					// 射撃
					attackCoolTimer_ = rangedWeapon_->Shoot(model_->GetTransform().translate, attackDirection_, bulletManager, context, true);
					overheatCount_++;
				} else {
					attackCoolTimer_--;

					// 倍速
					if (isBoss_) {
						attackCoolTimer_--;

						weaponChangeTimer_--;
						if (weaponChangeTimer_ <= 0) {
							weaponChangeTimer_ = 300;
							if (weaponNum_ == 0) {
								multipleWeapons_[0] = std::move(rangedWeapon_);
								rangedWeapon_ = std::move(multipleWeapons_[1]);
								weaponNum_ = 1;
							} else {
								multipleWeapons_[1] = std::move(rangedWeapon_);
								rangedWeapon_ = std::move(multipleWeapons_[0]);
								weaponNum_ = 0;
							}
						}
					}
				}

				if (overheatCount_ >= overheat_) {
					overheatCount_ = 0;
					attackCoolTimer_ = 120;
				}
			}

			// 攻撃前警告
			if (attackCoolTimer_ <= attackMotionStart_) {
				float sizeEase;
				if (attackCoolTimer_ < attackMotionStart_ / 2) {
					float t = (1 - float(attackMotionStart_ - attackCoolTimer_) / float(attackMotionStart_)) * 2;
					sizeEase = EaseIn(1, 1.7f, t);
				} else {
					float t = float(attackMotionStart_ - attackCoolTimer_) / float(attackMotionStart_) * 2;
					sizeEase = EaseIn(1, 1.7f, t);
				}
				model_->SetScale({ sizeEase,sizeEase,sizeEase });
			}
		} else {
			stunTimer_--;

			// ノックバック
			model_->SetScale({ 1,1,1 });
			float length = Length(velocity_);
			length -= 0.05f;
			if (length < 0) { length = 0; }
			velocity_ = Normalize(velocity_) * length;

			// 速度をもとに移動
			Vector2 pos = { model_->GetTransform().translate.x,model_->GetTransform().translate.z };
			for (int i = 0; i < 3; ++i) { // 3回に分ける
				pos.x += velocity_.x / 3.0f;
				mapCheck->ResolveCollisionX(pos, status_.radius, true);
				pos.y += velocity_.z / 3.0f;
				mapCheck->ResolveCollisionY(pos, status_.radius, true);
			}

			if (stunTimer_ <= 0 && !status_.canFly) { isFall_ = mapCheck->IsFall(pos, status_.radius); }
			model_->SetTranslate({ pos.x,model_->GetTransform().translate.y,pos.y });
		}
	} else {
		// 落下
		model_->SetTranslate(model_->GetTransform().translate - Vector3{ 0,0.7f,0 });
		if (model_->GetTransform().translate.y < -10.0f) {
			isDead_ = true;
		}
	}
}

void Enemy::Draw(GameContext* context, Camera* camera) {
	context->DrawEntity(*model_, *camera);
}

void Enemy::Hit(int damage, Vector3 from) {
	status_.hp -= damage;
	if (status_.hp <= 0) { isDead_ = true; }

	if (!isBoss_) {
		// 行動不能
		stunTimer_ = 10;

		// ノックバック
		velocity_ = Normalize(model_->GetTransform().translate - from) * 0.3f;
	}

	// 強制的に発見
	if (target_ == nullptr) {
		searchRadius_ *= 10.0f;
	}

	// ダメージを受けたら赤くする
	for (auto& mesh : model_->GetModel()->GetMeshes()) {
		auto data = mesh->GetMaterial()->GetData();
		data.color = { 1.0f,0.2f,0.2f,1.0f };
		mesh->GetMaterial()->SetData(data);
	}
}

// EaseInBackの数値調整版
float Enemy::EaseIn(float start, float end, float t) {
	if (t > 1.0f)t = 1.0f;
	else if (t < 0.0f)t = 0.0f;

	float easedT = 1.0f - cosf((t * float(std::numbers::pi)) / 2.0f);
	return (1.0f - easedT) * start + easedT * end;
}