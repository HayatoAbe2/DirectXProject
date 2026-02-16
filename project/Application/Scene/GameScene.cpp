#include "GameScene.h"

GameScene::~GameScene() {
}

void GameScene::Initialize() {
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(context_);
	camera_ = std::make_unique<Camera>();
	camera_->transform_.rotate = { 0,0,0 };
	camera_->transform_.translate = { 0,0,-cameraDistance_ };

	playerModel_ = context_->LoadModel("Resources", "suzanne.obj");
}

void GameScene::Update() {
	debugCamera_->Update();
	camera_->Update(context_, debugCamera_.get());
}

void GameScene::Draw() {
	context_->DrawModel(playerModel_.get(), camera_.get());
}