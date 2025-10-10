#include "Player.h"
#include "GameContext.h"
#include "Model.h"
#include "Camera.h"
#include <numbers>

Player::~Player() {

}

void Player::Initialize(Model* playerModel) {
	model_ = playerModel;
	model_->SetTransform({ {1,1,1},{0,float(std::numbers::pi),0},{0,0,0} });
}

void Player::Update() {
	// 移動

}

void Player::Draw(GameContext* context,Camera* camera) {
	model_->UpdateTransformation(*camera);
	context->DrawModel(*model_,BlendMode::None);
}
