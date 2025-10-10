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
}

void GameScene::Initialize() {
	debugCamera_ = new DebugCamera;
	debugCamera_->Initialize(context_);
	camera_ = new Camera;
	camera_->transform_.translate = { 0,1,-20 };
	camera_->transform_.rotate = { -0.1f,0,0};

	playerModel_ = context_->LoadModel("Resources/fence", "fence.obj");
	player_ = new Player();
	player_->Initialize(playerModel_);
}

void GameScene::Update() {
	player_->Update();

	camera_->UpdateCamera(context_->GetWindowSize(), *debugCamera_);
	debugCamera_->Update();
}

void GameScene::Draw() {
	player_->Draw(context_,camera_);
}