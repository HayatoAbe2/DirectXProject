#include "Player.h"
#include "GameContext.h"
#include "Entity.h"
#include "BulletManager.h"
#include "Camera.h"
#include "MapCheck.h"
#include "ItemManager.h"
#include "ImGuiManager.h"
#include "Sprite.h"
#include "ParticleSystem.h"
#include "AssaultRifle.h"
#include "Shotgun.h"
#include "FireBall.h"
#include "Pistol.h"

#include <numbers>
#include <cmath>
#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"
#include "Windows.h"
#include "DirectXMath.h"

Player::~Player() {

}

void Player::Initialize(Entity* playerModel, GameContext* context) {
	model_ = playerModel;
	transform_.translate.x = 1;
	transform_.translate.z = 1;

	// 操作
	control_ = std::make_unique<Entity>();
	control_->SetSprite(context->LoadSprite("Resources/Control/leftClick.png"));
	control_->GetSprite()->SetSize({ 48,65 });
	control_->GetSprite()->SetPosition({ 640 - 24 + 100,710 - 65 });

	equipment_ = std::make_unique<Entity>();
	equipment_->SetSprite(context->LoadSprite("Resources/Control/equipmentAssaultRifle.png"));
	equipment_->GetSprite()->SetSize({ 120,120 });
	equipment_->GetSprite()->SetPosition({ 640 - 60,710 - 160 });

	life_ = std::make_unique<Entity>();
	life_->SetSprite(context->LoadSprite("Resources/UI/gauge.png"));
	life_->GetSprite()->SetSize({ 290,68 });
	life_->GetSprite()->SetPosition({ 10,10 });

	moveParticle_ = std::make_unique<Entity>();
	moveParticle_->SetParticleSystem(context->LoadInstancedModel("Resources/Particle/Fire", "fireEffect.obj", moveParticleNum_));
	moveParticle_->GetParticleSystem()->SetLifeTime(10);
	moveParticle_->GetParticleSystem()->SetColor({ 0.6f, 0.6f, 0.6f, 1.0f });
}

void Player::Update(GameContext* context, MapCheck* mapCheck, ItemManager* itemManager_, Camera* camera, BulletManager* bulletManager) {
	if (stunTimer_ <= 0) {
		// 入力方向
		Vector2 input = { 0,0 };

		// 移動
		input.x = context->GetLeftStick().x;
		input.y = context->GetLeftStick().y;

		if (context->IsPress(DIK_A)) {
			input.x = -1;
		}

		if (context->IsPress(DIK_D)) {
			input.x = 1;
		}

		if (context->IsPress(DIK_W)) {
			input.y = -1;
		}

		if (context->IsPress(DIK_S)) {
			input.y = 1;
		}

		// 入力を反映
		Vector2 move = Normalize(input) * moveSpeed_;
		velocity_.x = move.x;
		velocity_.z = -move.y;

		// 速度をもとに移動
		Vector2 pos = { transform_.translate.x,transform_.translate.z };
		pos.x += velocity_.x;
		mapCheck->ResolveCollisionX(pos, radius_,false);
		pos.y += velocity_.z;
		mapCheck->ResolveCollisionY(pos, radius_,false);
		transform_.translate.x = pos.x;
		transform_.translate.z = pos.y;

		if (velocity_.x != 0 || velocity_.z != 0) {
			// 移動による向き変更
			transform_.rotate.y = -std::atan2(velocity_.z, velocity_.x) + float(std::numbers::pi) / 2.0f;

			// パーティクル
			moveParticleEmitTimer_++;
			if (moveParticleEmitTimer_ >= moveParticleEmitInterval_) {
				for (int i = 0; i < 3; ++i) {
					Vector3 randomVector = {
					context->RandomFloat(-moveParticleRange_ / 2.0f, moveParticleRange_ / 2.0f),
					-0.5f,
					context->RandomFloat(-moveParticleRange_ / 2.0f, moveParticleRange_ / 2.0f),
					};
					Transform transform = model_->GetTransform();
					transform.translate += randomVector;
					transform.scale = { 1.0f,1.0f,1.0f };
					moveParticle_->GetParticleSystem()->Emit(transform, -velocity_ * 0.4f);
				}
				moveParticleEmitTimer_ = 0;
			}
		}
		moveParticle_->GetParticleSystem()->Update();

		// アイテム取得
		if (context->IsTrigger(DIK_F)) {
			itemManager_->Interact(this);
			if(rangedWeapon_){

                if (rangedWeapon_ && dynamic_cast<AssaultRifle*>(rangedWeapon_.get())) {
                   equipment_->SetSprite(context->LoadSprite("Resources/Control/equipmentAssaultRifle.png"));
                } else if (rangedWeapon_ && dynamic_cast<Pistol*>(rangedWeapon_.get())) {
                   equipment_->SetSprite(context->LoadSprite("Resources/Control/equipmentPistol.png"));
                } else if (rangedWeapon_ && dynamic_cast<Shotgun*>(rangedWeapon_.get())) {
                   equipment_->SetSprite(context->LoadSprite("Resources/Control/equipmentShotgun.png"));
                } else if (rangedWeapon_ && dynamic_cast<FireBall*>(rangedWeapon_.get())) {
                   equipment_->SetSprite(context->LoadSprite("Resources/Control/equipmentSpellbook.png"));
                }
                equipment_->GetSprite()->SetSize({120, 120});
                equipment_->GetSprite()->SetPosition({640 - 60, 710 - 160});
			}
		}

		// 攻撃の向き
		if (context->IsClickLeft()) {
			Vector2 win = context->GetWindowSize();
			Vector2 mouse = context->GetMousePosition();
			mouse.y = win.y - mouse.y;

			Vector2 dir = Normalize(mouse - win / 2.0f);
			attackDirection_ = { dir.x,0,dir.y };

			// プレイヤーの向き
			transform_.rotate.y = -std::atan2(attackDirection_.z, attackDirection_.x) + float(std::numbers::pi) / 2.0f;

			// 減速
			if (rangedWeapon_) {
				moveSpeed_ = defaultMoveSpeed_ * (1.0f - rangedWeapon_->GetStatus().weight);
			}
		} else {
			moveSpeed_ = defaultMoveSpeed_;
		}

		if (rangedWeapon_) {
			rangedWeapon_->Update();
			weaponTransform_ = transform_;
			weaponTransform_.translate += {std::sin(transform_.rotate.y), 0, std::cos(transform_.rotate.y)}; // 前方に配置
		}

		// 射撃
		if (shootCoolTime_ <= 0) {
			if (context->IsClickLeft()) {
				if (rangedWeapon_) {
					shootCoolTime_ = rangedWeapon_->Shoot(weaponTransform_.translate, attackDirection_, bulletManager, context, false);
				}
			}

		} else {
			shootCoolTime_--;
		}
	} else {
		stunTimer_--;
		shootCoolTime_--;
	}
}

void Player::Draw(GameContext* context, Camera* camera) {

	model_->SetTransform(transform_);
	context->DrawEntity(*model_, *camera);

	if (rangedWeapon_) {
		rangedWeapon_->GetWeaponModel()->SetTransform(weaponTransform_);
		context->DrawEntity(*rangedWeapon_->GetWeaponModel(), *camera);
		context->DrawEntity(*control_, *camera);
		context->DrawEntity(*equipment_, *camera);
	}

	life_->GetSprite()->SetTextureRect(0, 0, (float(hp_) / float(maxHp_)) * 290, 68);
	life_->GetSprite()->SetSize({ (float(hp_) / float(maxHp_)) * 290,68 });
	context->DrawEntity(*life_, *camera);

	// パーティクル
	context->DrawEntity(*moveParticle_, *camera, BlendMode::Add);

#ifdef USE_IMGUI
	ImGui::Begin("Player Info");
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", transform_.translate.x, transform_.translate.y, transform_.translate.z);
	ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", transform_.rotate.x, transform_.rotate.y, transform_.rotate.z);
	ImGui::Text("Scale: (%.2f, %.2f, %.2f)", transform_.scale.x, transform_.scale.y, transform_.scale.z);
	ImGui::Text("HP: %d", hp_);
	ImGui::End();
#endif
}

void Player::Hit(int damage, Vector3 from) {
	hp_ -= damage;
	if (hp_ <= 0) {
		// ゲームオーバー
	}


}

void Player::SetWeapon(std::unique_ptr<RangedWeapon> rangedWeapon) { rangedWeapon_ = std::move(rangedWeapon); }