#include "GameScene.h"
#include "MathUtils.h"
#include "GameContext.h"
#include "Player.h"
#include "Entity.h"
#include "Model.h"
#include "Sprite.h"
#include "InstancedModel.h"
#include "ParticleSystem.h"
#include "MapTile.h"
#include "MapCheck.h"
#include "WeaponManager.h"
#include "ItemManager.h"

#include <numbers>

GameScene::~GameScene() {
	delete camera_;
	delete debugCamera_;
	delete player_;
	delete mapTile_;
	delete mapCheck_;
	delete weaponManager_;
}

void GameScene::Initialize() {
	debugCamera_ = new DebugCamera;
	debugCamera_->Initialize(context_);
	camera_ = new Camera;
	camera_->transform_.translate = { 0,3,-10 };
	camera_->transform_.rotate = { 0.2f,0,0};

	playerModel_ = std::make_unique<Entity>();
	playerModel_->SetModel(context_->LoadModel("Resources/Player", "player.obj"));

	// マップ
	wall_ = std::make_unique<Entity>();
	wall_->SetInstancedModel(context_->LoadInstancedModel("Resources/Block", "block.obj", 100));

	floor_ = std::make_unique<Entity>();
	floor_->SetInstancedModel(context_->LoadInstancedModel("Resources/Floor", "floor.obj", 100));

	mapTile_ = new MapTile();
	mapTile_->Initialize(wall_.get(), floor_.get());
	mapTile_->LoadCSV("Resources/mapData.csv");

	// マップ判定
	mapCheck_ = new MapCheck();
	mapCheck_->Initialize(mapTile_->GetMap(), mapTile_->GetTileSize());

	// 武器マネージャー
	weaponManager_ = new WeaponManager();
	weaponManager_->Initilaize(context_);

	// アイテムマネージャー
	itemManager_ = new ItemManager();
	itemManager_->Initialize(weaponManager_);

	// プレイヤー
	player_ = new Player();
	player_->Initialize(playerModel_.get());

	// 天球
	skydome_ = std::make_unique<Entity>();
	skydome_->SetModel(context_->LoadModel("Resources/Skydome", "skydome.obj",false));
}

void GameScene::Update() {
	// プレイヤー処理
	player_->Update(context_,mapCheck_,itemManager_,bullets_);
	
	// カメラ追従
	camera_->transform_.translate = player_->GetTransform().translate + Vector3{0,0,-cameraDistance_};
	camera_->UpdateCamera(context_->GetWindowSize(), *debugCamera_);
	debugCamera_->Update();

	// 弾の処理
	for (const auto &bullet : bullets_) {
		bullet->Update();
	}
}

void GameScene::Draw() {
	context_->DrawEntity(*skydome_, *camera_);
	mapTile_->Draw(context_, camera_);
	player_->Draw(context_,camera_);
	
	for (const auto& bullet : bullets_) {
		bullet->Draw(context_,camera_);
	}
}