#include "Player.h"
#include "GameContext.h"
#include "Model.h"
#include "Camera.h"
#include <numbers>
#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"

Player::~Player() {

}

void Player::Initialize(Model* playerModel) {
	model_ = playerModel;
	model_->SetTransform(transform_);
}

void Player::Update(GameContext* context) {
	// 移動

	input_.x = context->GetLeftStick().x * moveSpeed_;
	input_.y = context->GetLeftStick().y * moveSpeed_;

	if (context->IsPress(DIK_A)) { input_.x = -moveSpeed_; }
	if (context->IsPress(DIK_D)) { input_.x = moveSpeed_; }
	if (context->IsPress(DIK_W)) { input_.y = -moveSpeed_; }
	if (context->IsPress(DIK_S)) { input_.y = moveSpeed_; }

	velocity_.x = input_.x;
	velocity_.z = -input_.y;
	transform_.translate += velocity_;

	if (context->IsPress(DIK_UP) || context->IsControllerPress(5)) { transform_.scale += {0.01f, 0.01f, 0.01f}; }
	if (context->IsPress(DIK_DOWN) || context->IsControllerPress(4)) { transform_.scale -= {0.01f, 0.01f, 0.01f}; }
	if (context->IsPress(DIK_LEFT) ) { transform_.rotate.y += 0.02f; }
	if (context->IsPress(DIK_RIGHT)) { transform_.rotate.y -= 0.02f; }

}

void Player::Draw(GameContext* context, Camera* camera) {
	model_->SetTransform(transform_);
	model_->UpdateTransformation(*camera);
	context->DrawModel(*model_, BlendMode::None);
}
