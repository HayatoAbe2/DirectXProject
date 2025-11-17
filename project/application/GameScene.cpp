#include "GameScene.h"
#include "MathUtils.h"
#include "GameContext.h"
#include "Player.h"
#include "EnemyManager.h"
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

#include "ImGuiManager.h"

GameScene::~GameScene() {
	delete camera_;
	delete debugCamera_;
	delete player_;
	delete enemyManager_;
	delete mapTile_;
	delete mapCheck_;
	delete weaponManager_;
}

void GameScene::Initialize() {
	debugCamera_ = new DebugCamera;
	debugCamera_->Initialize(context_);
	camera_ = new Camera;
	camera_->transform_.rotate = { float(std::numbers::pi) / 2.0f,0,0};

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

	// 敵
	enemyManager_ = new EnemyManager();
	enemyManager_->Initialize(context_);

	// 天球
	skydome_ = std::make_unique<Entity>();
	skydome_->SetModel(context_->LoadModel("Resources/Skydome", "skydome.obj",false));
	camera_->transform_.translate = player_->GetTransform().translate + Vector3{ 0,0,-cameraDistance_ };

}

void GameScene::Update() {
	// プレイヤー処理
	player_->Update(context_,mapCheck_,itemManager_,camera_,bullets_);
	
	// カメラ追従
	camera_->transform_.translate = player_->GetTransform().translate + Vector3{0,50,0};
	camera_->UpdateCamera(context_->GetWindowSize(), *debugCamera_);
	debugCamera_->Update(); 

	// 敵
	enemyManager_->Update(context_, mapCheck_, player_, bullets_);

	// 弾の処理
	for (const auto &bullet : bullets_) {
		bullet->Update();
	}
}

void GameScene::Draw() {
	context_->DrawEntity(*skydome_, *camera_);
	mapTile_->Draw(context_, camera_);
	player_->Draw(context_,camera_);
	enemyManager_->Draw(context_, camera_);
	
	for (const auto& bullet : bullets_) {
		bullet->Draw(context_,camera_);
	}

#ifdef USE_IMGUI
	ImGui::Begin("Player Info");
	ImGui::DragFloat3("rot", &camera_->transform_.rotate.x, 0.1f);
	ImGui::DragFloat3("tra", &camera_->transform_.translate.x, 0.1f);
	ImGui::End();
#endif
}