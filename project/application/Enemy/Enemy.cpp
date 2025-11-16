#include "Enemy.h"
#include "Entity.h"
#include "GameContext.h"
#include "Camera.h"
#include "Bullet.h"
#include "MapCheck.h"
#include "Player.h"
#include "WeaponStatus.h"

#include <numbers>
#include <cmath>

Enemy::Enemy(std::unique_ptr<Entity> model, Vector3 pos) {
	model_ = std::move(model);
	model_->SetTranslate(pos);

	/*std::unique_ptr<Entity> fireBall = std::make_unique<Entity>();
	fireBall->SetModel(context->LoadModel("Resources/Weapons", "pistol.obj"));
	weaponModels_.push_back(std::move(fireBall));
	bulletStatus_.damage = 3;
	bulletStatus_.weight = 0.01f;
	bulletStatus_.bulletSize = 1.0f;
	bulletStatus_.bulletSpeed = 0.2f;
	bulletStatus_.shootCoolTime = 25;
	bulletStatus_.weaponModel = ;
	bulletStatus_.bulletDirectoryPath = "Resources/Bullets";
	bulletStatus_.bulletFileName = "fireBall.obj";
	*/
}

void Enemy::Update(GameContext* context, MapCheck* mapCheck, Player* player,std::vector<std::unique_ptr<Bullet>>& bullets) {
	// 移動
	Vector3 velocity{};

	// 速度をもとに移動
	Vector2 pos = { model_->GetTransform().translate.x,model_->GetTransform().translate.z };
	pos.x += velocity.x;
	mapCheck->ResolveCollisionX(pos, radius_);
	pos.y += velocity.z;
	mapCheck->ResolveCollisionY(pos, radius_);
	model_->SetTranslate({ pos.x,model_->GetTransform().translate.y,pos.y});

	if (velocity.x != 0 || velocity.z != 0) {
		model_->SetRotate({ 0,-std::atan2(velocity.z, velocity.x) + float(std::numbers::pi) / 2.0f,0 });
	}

	if(Length(player->GetTransform().translate - model_->GetTransform().translate) < searchRadius_){
		target_ = player;
	} else {
		target_ = nullptr;
	}

	if (target_) {
		// 攻撃の向き
		attackDirection_ = Normalize(target_->GetTransform().translate - model_->GetTransform().translate);
		model_->SetRotate({ 0,-std::atan2(attackDirection_.z, attackDirection_.x) + float(std::numbers::pi) / 2.0f,0 });
 
	}

	if (attackCoolTime_ <= 0) {
		// 射撃
		/*if (target_) {

			auto bulletModel = std::make_unique<Entity>();
			bulletModel->SetTranslate(model_->GetTransform().translate);
			bulletModel->SetModel(context->LoadModel("Resources/Bullets/FireBall","fireBall.obj"));

			std::unique_ptr newBullet = std::make_unique<Bullet>(std::move(bulletModel), attackDirection_* bulletSpeed_, status);
			bullets.push_back(std::move(newBullet));
			return status_.shootCoolTime;
		}*/

	} else {
		attackCoolTime_--;
	}
}

void Enemy::Draw(GameContext* context, Camera* camera) {
	context->DrawEntity(*model_, *camera);
}