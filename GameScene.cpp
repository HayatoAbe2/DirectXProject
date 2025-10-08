#include "GameScene.h"
#include "Graphics.h"
#include "Input.h"
#include "Model.h"
#include "Camera.h"
#include "DebugCamera.h"
#include "MathUtils.h"
#include "Audio.h"
#include "GameContext.h"
#include <numbers>

GameScene::~GameScene() {
	delete camera_;
	delete debugCamera_;
}

void GameScene::Initialize() {
	debugCamera_ = new DebugCamera;
	debugCamera_->Initialize();
	camera_ = new Camera;
	camera_->transform_.translate = { 0,0,-50 };

	playerModel_ = context_->LoadModel("Resources", "bunny.obj");
}

void GameScene::Update() {
	if (context_->IsTrigger(DIK_R)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
	debugCamera_->SetEnable(isDebugCameraActive_);
	camera_->UpdateCamera(context_->GetWindowSize(), *debugCamera_);
	playerModel_->SetTransform({ {1,1,1},{0,float(std::numbers::pi),0},{0,0,0} });
}

void GameScene::Draw() {
	playerModel_->UpdateTransformation(*camera_);
	context_->DrawModel(*playerModel_);
}