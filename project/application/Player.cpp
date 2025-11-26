#include "Player.h"
#include "GameContext.h"
#include "Entity.h"
#include "BulletManager.h"
#include "Camera.h"
#include "MapCheck.h"
#include "ItemManager.h"
#include "ImGuiManager.h"
#include "Sprite.h"

#include <numbers>
#include <cmath>
#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"

Player::~Player() {

}

void Player::Initialize(Entity* playerModel,GameContext* context) {
	model_ = playerModel;
	transform_.translate.x = 1;
	transform_.translate.z = 1;

	// 操作
	control_ = std::make_unique<Entity>();
	control_->SetSprite(context->LoadSprite("Resources/Control/control.png"));
	control_->GetSprite()->SetSize({ 331,39 });
	control_->GetSprite()->SetPosition({ 640 - 150,710 - 39 });

	life_ = std::make_unique<Entity>();
	life_->SetSprite(context->LoadSprite("Resources/UI/gauge.png"));
	life_->GetSprite()->SetSize({ 290,68 });
	life_->GetSprite()->SetPosition({ 10,10 });
}

void Player::Update(GameContext* context, MapCheck* mapCheck, ItemManager* itemManager_, Camera* camera,BulletManager* bulletManager) {
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
		mapCheck->ResolveCollisionX(pos, radius_);
		pos.y += velocity_.z;
		mapCheck->ResolveCollisionY(pos, radius_);
		transform_.translate.x = pos.x;
		transform_.translate.z = pos.y;

		if (velocity_.x != 0 || velocity_.z != 0) {
			// 移動による向き変更
			transform_.rotate.y = -std::atan2(velocity_.z, velocity_.x) + float(std::numbers::pi) / 2.0f;
		}

		// アイテム取得
		if (context->IsPress(DIK_F)) {
			itemManager_->Interact(this);
		}

		// 攻撃の向き
		Vector2 win = context->GetWindowSize();
		Vector2 mouse = context->GetMousePosition();
		mouse.y = win.y - mouse.y; // y反転
		Vector2 dir = Normalize(mouse - win / 2.0f);
		attackDirection_ = { dir.x,0,dir.y };

		if (rangedWeapon_) {
			rangedWeapon_->Update();
		}

		if (context->IsClickLeft()) {
			// 攻撃の向き
			transform_.rotate.y = -std::atan2(attackDirection_.z, attackDirection_.x) + float(std::numbers::pi) / 2.0f;

			if (rangedWeapon_) {
				moveSpeed_ = defaultMoveSpeed_ * (1.0f - rangedWeapon_->GetStatus().weight);
			}
		} else {
			moveSpeed_ = defaultMoveSpeed_;
		}

		if (shootCoolTime_ <= 0) {
			// 射撃
			if (context->IsClickLeft()) {
				if (rangedWeapon_) {
					shootCoolTime_ = rangedWeapon_->Shoot(transform_.translate, attackDirection_, bulletManager, context, false);
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
	context->DrawEntity(*model_,*camera);

	if (rangedWeapon_) {
		Transform transform = transform_;
		transform.translate.y += 0.5f;
		rangedWeapon_->GetWeaponModel()->SetTransform(transform);
		context->DrawEntity(*rangedWeapon_->GetWeaponModel(), *camera);
		context->DrawEntity(*control_, *camera);
	}

	life_->GetSprite()->SetTextureRect(0, 0, (float(hp_) / float(maxHp_)) * 290, 68);
	life_->GetSprite()->SetSize({ (float(hp_) / float(maxHp_)) * 290,68 });
	context->DrawEntity(*life_, *camera);

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