#include "Player.h"
#include "GameContext.h"
#include "Entity.h"
#include "PlayerBullet.h"
#include "Camera.h"
#include <numbers>
#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

Player::~Player() {

}

void Player::Initialize(Entity* playerModel) {
	model_ = playerModel;
}

void Player::Update(GameContext* context) {
	// 移動
	input_.x = context->GetLeftStick().x * moveSpeed_;
	input_.y = context->GetLeftStick().y * moveSpeed_;

	if (context->IsPress(DIK_A)) { 
		input_.x = -moveSpeed_;
	}

	if (context->IsPress(DIK_D)) {
		input_.x = moveSpeed_; 
	}
	
	if (context->IsPress(DIK_W)) {
		input_.y = -moveSpeed_; 
	}

	if (context->IsPress(DIK_S)) {
		input_.y = moveSpeed_; 
	}

	// 入力を反映
	velocity_.x = input_.x;
	velocity_.z = -input_.y;

	// 速度をもとに移動
	transform_.translate += velocity_;

	if (context->IsPress(DIK_UP) || context->IsControllerPress(5)) { transform_.scale += {0.01f, 0.01f, 0.01f}; }
	if (context->IsPress(DIK_DOWN) || context->IsControllerPress(4)) { transform_.scale -= {0.01f, 0.01f, 0.01f}; }
	if (context->IsPress(DIK_LEFT) ) { transform_.rotate.y += 0.03f; }
	if (context->IsPress(DIK_RIGHT)) { transform_.rotate.y -= 0.03f; }

	
}

void Player::Draw(GameContext* context, Camera* camera) {
	model_->SetTransform(transform_);
	context->DrawEntity(*model_,*camera);

	/*ImGui::Begin("Player Info");
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", transform_.translate.x, transform_.translate.y, transform_.translate.z);
	ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", transform_.rotate.x, transform_.rotate.y, transform_.rotate.z);
	ImGui::Text("Scale: (%.2f, %.2f, %.2f)", transform_.scale.x, transform_.scale.y, transform_.scale.z);
	ImGui::End();*/
}
