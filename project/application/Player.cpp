#include "Player.h"
#include "GameContext.h"
#include "Entity.h"
#include "Bullet.h"
#include "Camera.h"
#include "MapCheck.h"
#include "ItemManager.h"

#include <numbers>
#include <cmath>
#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

Player::~Player() {

}

void Player::Initialize(Entity* playerModel) {
	model_ = playerModel;
	transform_.translate.x = 1;
	transform_.translate.z = 1;
}

void Player::Update(GameContext* context, MapCheck* mapCheck, ItemManager* itemManager_, std::vector<std::unique_ptr<Bullet>>& bullets) {
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
		transform_.rotate.y = -std::atan2(velocity_.z, velocity_.x) + float(std::numbers::pi) / 2.0f;
	}

	if (context->IsPress(DIK_UP) || context->IsControllerPress(5)) { transform_.scale += {0.01f, 0.01f, 0.01f}; }
	if (context->IsPress(DIK_DOWN) || context->IsControllerPress(4)) { transform_.scale -= {0.01f, 0.01f, 0.01f}; }
	if (context->IsPress(DIK_LEFT)) { transform_.rotate.y += 0.03f; }
	if (context->IsPress(DIK_RIGHT)) { transform_.rotate.y -= 0.03f; }

	// アイテム取得
	if (context->IsPress(DIK_E)) {
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

	if (shootCoolTime_ <= 0) {
		// 射撃
		if (context->IsClickLeft()) {
			if (rangedWeapon_) {
				rangedWeapon_->Shoot(transform_.translate,attackDirection_,bullets,context);
			}
		}

	}else{
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
	}

	/*ImGui::Begin("Player Info");
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", transform_.translate.x, transform_.translate.y, transform_.translate.z);
	ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", transform_.rotate.x, transform_.rotate.y, transform_.rotate.z);
	ImGui::Text("Scale: (%.2f, %.2f, %.2f)", transform_.scale.x, transform_.scale.y, transform_.scale.z);
	ImGui::End();*/
}

void Player::SetWeapon(std::unique_ptr<RangedWeapon> rangedWeapon) { rangedWeapon_ = std::move(rangedWeapon); }