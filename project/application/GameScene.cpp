#include "GameScene.h"
#include "Model.h"
#include "Camera.h"
#include "DebugCamera.h"
#include "MathUtils.h"
#include "GameContext.h"
#include "Player.h"
#include <numbers>

GameScene::~GameScene() {
	delete camera_;
	delete debugCamera_;
	delete player_;
	delete playerModel_;
}

void GameScene::Initialize() {
	debugCamera_ = new DebugCamera;
	debugCamera_->Initialize();
	camera_ = new Camera;
	camera_->transform_.translate = { 0,20,-30 };
	camera_->transform_.rotate = {};

	playerModel_ = context_->LoadModel("Resources", "bunny.obj");
	player_ = new Player();
	player_->Initialize(playerModel_);
}

void GameScene::Update() {
	player_->Update();

	if (context_->IsTrigger(DIK_R)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
	debugCamera_->SetEnable(isDebugCameraActive_);
	camera_->UpdateCamera(context_->GetWindowSize(), *debugCamera_);
}

void GameScene::Draw() {
	player_->Draw(context_,camera_);
}