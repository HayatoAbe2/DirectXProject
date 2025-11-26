#include "Enemy.h"
#include "Entity.h"
#include "GameContext.h"
#include "Camera.h"
#include "BulletManager.h"
#include "MapCheck.h"
#include "Player.h"
#include "Mesh.h"

#include <numbers>
#include <cmath>

Enemy::Enemy(std::unique_ptr<Entity> model, Vector3 pos, std::unique_ptr<RangedWeapon> rWeapon) {
	model_ = std::move(model);
	model_->SetTranslate(pos);
	rangedWeapon_ = std::move(rWeapon);
}

void Enemy::Update(GameContext* context, MapCheck* mapCheck, Player* player, BulletManager* bulletManager) {
	for (auto& mesh : model_->GetModel()->GetMeshes()) {
		auto data = mesh->GetMaterial()->GetData();
		data.color = { 1.0f,1.0f,1.0f,1.0f };
		mesh->GetMaterial()->SetData(data);
	}

	if (stunTimer_ <= 0) {
		// 移動
		if (target_) {
			actionChangeTimer_++;
			if (actionChangeTimer_ > actionChangeInterval_) {
				actionChangeTimer_ = 0;

				Vector2 direction = Normalize(Vector2{ context->RandomFloat(-1,1),context->RandomFloat(-1,1) });
				velocity_.x = direction.x * moveSpeed_;
				velocity_.z = direction.y * moveSpeed_;
			}
		} else {
			actionChangeTimer_ = 0;
			velocity_ = { 0,0,0 };
		}

		// 速度をもとに移動
		Vector2 pos = { model_->GetTransform().translate.x,model_->GetTransform().translate.z };
		pos.x += velocity_.x;
		mapCheck->ResolveCollisionX(pos, radius_);
		pos.y += velocity_.z;
		mapCheck->ResolveCollisionY(pos, radius_);
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
			if (loseSightTimer_ > loseSightTime_) {
				target_ = nullptr;
			}
		}

		if (target_) {
			// 攻撃の向き
			attackDirection_ = Normalize(target_->GetTransform().translate - model_->GetTransform().translate);
			model_->SetRotate({ 0,-std::atan2(attackDirection_.z, attackDirection_.x) + float(std::numbers::pi) / 2.0f,0 });

			if (loseSightRadius_ < Length(target_->GetTransform().translate - model_->GetTransform().translate)) {
				target_ = nullptr;
				searchRadius_ = defaultSearchRadius_;
			}
		}

		if (attackCoolTimer_ <= 0) {
			// 射撃
			if (target_) {
				attackCoolTimer_ = rangedWeapon_->Shoot(model_->GetTransform().translate, attackDirection_, bulletManager, context, true);
			}
		} else {
			attackCoolTimer_--;
		}
	} else {
		stunTimer_--;

		// ノックバック
		model_->SetTranslate(model_->GetTransform().translate + velocity_);
		float length = Length(velocity_);
		length -= 0.05f;
		if (length < 0) { length = 0; }
		velocity_ = Normalize(velocity_) * length;
	}
}

void Enemy::Draw(GameContext* context, Camera* camera) {
	context->DrawEntity(*model_, *camera);
}

void Enemy::Hit(int damage, Vector3 from) {
	hp_ -= damage; 
	if (hp_ <= 0) { isDead_ = true; }
	
	stunTimer_ = 10;

	// ノックバック
	velocity_ = Normalize(model_->GetTransform().translate - from) * 0.3f;


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
