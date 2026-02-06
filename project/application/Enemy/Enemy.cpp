#include "Enemy.h"
#include "Camera.h"
#include "MapCheck.h"
#include "Player.h"
#include "Mesh.h"
#include "EnemyStatus.h"

#include <numbers>
#include <cmath>

Enemy::Enemy(std::unique_ptr<Model> model, std::unique_ptr<Model> shadowModel,
	Vector3 pos, EnemyStatus status, std::unique_ptr<Weapon> rWeapon) {
	model_ = std::move(model);
	model_->SetTranslate(pos);
	shadowModel_ = std::move(shadowModel);
	auto matData = shadowModel_->GetMaterial(0)->GetData();
	matData.color = { 0,0,0,1 };
	shadowModel_->GetMaterial(0)->SetData(matData);
	status_ = status;

	weapon_ = std::move(rWeapon);
}

Enemy::Enemy(std::unique_ptr<Model> model, std::unique_ptr<Model> shadowModel, Vector3 pos, EnemyStatus status, std::vector<std::unique_ptr<Weapon>> rWeapons) {
	model_ = std::move(model);
	model_->SetTranslate(pos);
	status_ = status;
	shadowModel_ = std::move(shadowModel);
	auto matData = shadowModel_->GetMaterial(0)->GetData();
	matData.color = { 0,0,0,1 };
	shadowModel_->GetMaterial(0)->SetData(matData);

	bossWeapons_ = std::move(rWeapons);
	weapon_ = std::move(bossWeapons_[0]);
}

void Enemy::Update(GameContext* context, MapCheck* mapCheck, Player* player, BulletManager* bulletManager, Camera* camera) {
	if (hitColorTime_) {
		hitColorTime_--;
		if (hitColorTime_ <= 0) {
			for (auto& mesh : model_->GetData()->meshes) {
				auto data = model_->GetMaterial(0)->GetData();
				data.color = { 1.0f,1.0f,1.0f,1.0f };
				model_->GetMaterial(0)->SetData(data);
				model_->GetMaterial(1)->SetData(data);
			}
		}
	}

	if (!isFall_) {

		if (stunTimer_ <= 0) {
			// 移動
			if (target_) { // 発見中

				if (rotateTimer_ >= rotateTime_) {
					// 方向転換の間隔
					rotateTimer_ = 0;
					rotateTime_ = context->RandomInt(minRotateTimer_, maxRotateTimer_);

					float length = Length(target_->GetTransform().translate - model_->GetTransform().translate);

					if (length > minDistance_) {
						// プレイヤー方向に移動
						Vector3 targetDir = Normalize(target_->GetTransform().translate - model_->GetTransform().translate);
						velocity_ = Vector3{ targetDir.x,0,targetDir.z } *status_.moveSpeed;
					} else {

						Vector2 direction = Normalize(Vector2{ context->RandomFloat(-1,1),context->RandomFloat(-1,1) });
						velocity_.x = direction.x * status_.moveSpeed;
						velocity_.z = direction.y * status_.moveSpeed;
					}
				}

			} else {
				Wait(context);

				if (targetAutoFound_) {
					target_ = player;
					targetAutoFound_ = false;
				}
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
				if (mapCheck->EnemyCanSeePlayer(model_->GetTransform().translate, player->GetTransform().translate)) {
					target_ = player;
					loseSightTimer_ = 0;
				}
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

				Attack(weapon_.get(), bulletManager, context, camera);
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
			Stun(context, mapCheck);
		}
	} else {
		Fall();
	}
}

void Enemy::Wait(GameContext* context) {
	// プレイヤーを見つけてない
	if (isMoving_) {
		randomTimer_++;
		if (randomTimer_ >= randomMoveTime_) {
			randomTimer_ = 0;
			isMoving_ = false;
			randomStopTime_ = context->RandomInt(minRandomStopTime_, maxRandomStopTime_);
			velocity_ = {};
		}
	} else {
		randomTimer_++;
		if (randomTimer_ >= randomStopTime_) {
			randomTimer_ = 0;
			isMoving_ = true;
			randomMoveTime_ = context->RandomInt(minRandomMoveTime_, maxRandomMoveTime_);

			Vector2 direction = Normalize(Vector2{ context->RandomFloat(-1,1),context->RandomFloat(-1,1) });
			velocity_.x = direction.x * status_.moveSpeed / 3.0f;
			velocity_.z = direction.y * status_.moveSpeed / 3.0f;
		}
	}
}

void Enemy::Stun(GameContext* context, MapCheck* mapCheck) {
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

	if (stunTimer_ <= 0 && !status_.canFly) {
		isFall_ = mapCheck->IsFall(pos);
		if (isFall_) { context->SoundPlay(L"Resources/Sounds/SE/fall.mp3", false); }
	}
	model_->SetTranslate({ pos.x,model_->GetTransform().translate.y,pos.y });
}

void Enemy::Fall() {
	// 落下
	model_->SetTranslate(model_->GetTransform().translate - Vector3{ 0,0.7f,0 });
	if (model_->GetTransform().translate.y < -10.0f) {
		isDead_ = true;
	}
}

void Enemy::Draw(GameContext* context, Camera* camera) {
	// 影描画
	if (!isFall_) {
		Transform shadowTransform = model_->GetTransform();
		shadowTransform.scale.y = 0.0f;
		shadowTransform.translate.y = 0.01f;
		shadowModel_->SetTransform(shadowTransform);
		context->DrawModel(shadowModel_.get(), camera);
	}
	context->DrawModel(model_.get(), camera);
}

void Enemy::Hit(float damage, Vector3 from, const float knockback) {

	status_.hp -= damage;
	if (status_.hp <= 0) { isDead_ = true; }

	if (status_.stunResist < 10) {
		// 行動不能
		stunTimer_ = 10 - status_.stunResist;

		// ノックバック
		velocity_ = Normalize(model_->GetTransform().translate - from) * knockback;
	}

	// 強制的に発見
	if (target_ == nullptr) {
		targetAutoFound_ = true;
	}

	// ダメージを受けたら赤くする
	auto data = model_->GetMaterial(0)->GetData();
	data.color = { 1.0f,0.2f,0.2f,1.0f };
	model_->GetMaterial(0)->SetData(data);
	model_->GetMaterial(1)->SetData(data);
	hitColorTime_ = 3;
}

// EaseInBackの数値調整版
float Enemy::EaseIn(float start, float end, float t) {
	if (t > 1.0f)t = 1.0f;
	else if (t < 0.0f)t = 0.0f;

	float easedT = 1.0f - cosf((t * float(std::numbers::pi)) / 2.0f);
	return (1.0f - easedT) * start + easedT * end;
}