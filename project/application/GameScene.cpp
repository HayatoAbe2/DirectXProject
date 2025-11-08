#include "GameScene.h"
#include "MathUtils.h"
#include "GameContext.h"
#include "Player.h"
#include "Entity.h"
#include "Model.h"
#include "Sprite.h"
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

	playerModel_ = std::make_unique<Entity>(context_->LoadModel("Resources", "teapot.obj"));
	player_ = new Player();
	player_->Initialize(playerModel_.get());

	// スプライト
	uvChecker_ = std::make_unique<Entity>(context_->LoadSprite("Resources/uvChecker.png"));
	uvChecker_->GetSprite()->SetSize({ 256.0f, 256.0f });
	uvChecker_->GetSprite()->SetTextureRect(0, 0, 128, 128);

	
	std::shared_ptr<Sprite> sprite = context_->LoadSprite("Resources/monsterBall.png");
	sprite->SetSize({ 256.0f, 256.0f });
	sprite->SetPosition({ 256.0f,0.0f });
	sprite_ = std::make_unique<Entity>(sprite);
}

void GameScene::Update() {
	player_->Update(context_);

	camera_->UpdateCamera(context_->GetWindowSize(), *debugCamera_);
	debugCamera_->Update();

	for (int i = 0; i < 10; ++i) {
		planeTransforms_[i] = {
			{1,1,1},{0,float(std::numbers::pi),0},{2.0f + i * 0.1f,i * 0.1f,i * 0.1f}
		};
	}
}

void GameScene::Draw() {
	player_->Draw(context_,camera_);
	context_->DrawEntity(*uvChecker_,*camera_);
}