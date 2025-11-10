#include "GameScene.h"
#include "MathUtils.h"
#include "GameContext.h"
#include "Player.h"
#include "Entity.h"
#include "Model.h"
#include "Sprite.h"
#include "InstancedModel.h"
#include "ParticleSystem.h"

#include <numbers>

GameScene::~GameScene() {
	delete camera_;
	delete debugCamera_;
	delete player_;
}

void GameScene::Initialize() {
	std::mt19937 randomEngine_(randomDevice_());

	debugCamera_ = new DebugCamera;
	debugCamera_->Initialize(context_);
	camera_ = new Camera;
	camera_->transform_.translate = { 0,1,-20 };
	camera_->transform_.rotate = { -0.1f,0,0};

	playerModel_ = std::make_unique<Entity>();
	playerModel_->SetModel(context_->LoadModel("Resources", "teapot.obj"));

	player_ = new Player();
	player_->Initialize(playerModel_.get());

	// スプライト
	uvChecker_ = std::make_unique<Entity>(); 
	uvChecker_->SetSprite(context_->LoadSprite("Resources/uvChecker.png"));
	uvChecker_->GetSprite()->SetSize({ 256.0f, 256.0f });
	uvChecker_->GetSprite()->SetTextureRect(0, 0, 128, 128);

	planeModel_ = std::make_unique<Entity>();
	planeModel_->SetInstancedModel(context_->LoadInstancedModel("Resources", "plane.obj", 10));
	planeTransforms_.resize(numPlaneInstance_);

	particle_ = std::make_unique<Entity>();
	particle_->SetParticleSystem(context_->LoadInstancedModel("Resources", "plane.obj", 10));
}

void GameScene::Update() {
	player_->Update(context_);

	camera_->UpdateCamera(context_->GetWindowSize(), *debugCamera_);
	debugCamera_->Update();

	for (int i = 0; i < numPlaneInstance_; ++i) {
		planeTransforms_[i] = {
			{1,1,1},{0,float(std::numbers::pi),0},{2.0f + i * 0.1f,i * 0.1f,i * 0.1f}
		};
	}

	std::uniform_real_distribution<float> distribution(-5.0f, 5.0f);
	Transform transform = { {1.0f,1.0f,1.0f,},{}, {} };
	Vector3 velocity = {distribution(randomEngine_),distribution(randomEngine_) ,0};
	particle_->GetParticleSystem()->Emit(transform, velocity);
	particle_->GetParticleSystem()->Update(1.0f / 60.0f);
}

void GameScene::Draw() {
	player_->Draw(context_,camera_);
	context_->DrawEntity(*uvChecker_,*camera_);

	planeModel_->SetInstanceTransforms(planeTransforms_);
	planeModel_->GetInstancedModel()->UpdateInstanceTransform(*camera_, planeTransforms_);
	context_->DrawEntity(*planeModel_, *camera_);

	context_->DrawEntity(*particle_,*camera_);
}