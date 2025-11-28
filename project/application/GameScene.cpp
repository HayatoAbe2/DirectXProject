#include "GameScene.h"
#include "MathUtils.h"
#include "GameContext.h"
#include "Player.h"
#include "EnemyManager.h"
#include "BulletManager.h"
#include "EffectManager.h"
#include "Entity.h"
#include "Model.h"
#include "Sprite.h"
#include "InstancedModel.h"
#include "ParticleSystem.h"
#include "MapTile.h"
#include "MapCheck.h"
#include "WeaponManager.h"
#include "ItemManager.h"
#include "CollisionChecker.h"

#include <numbers>

#include "ImGuiManager.h"

GameScene::~GameScene() {
	delete camera_;
	delete debugCamera_;
	delete player_;
	delete enemyManager_;
	delete bulletManager_;
	delete mapTile_;
	delete mapCheck_;
	delete weaponManager_;
	delete itemManager_;
	delete collisionChecker_;
	delete effectManager_;
}

void GameScene::Initialize() {
	debugCamera_ = new DebugCamera;
	debugCamera_->Initialize(context_);
	camera_ = new Camera;
	camera_->transform_.rotate = { float(std::numbers::pi) / 2.0f,0,0 };

	playerModel_ = std::make_unique<Entity>();
	playerModel_->SetModel(context_->LoadModel("Resources/Player", "player.obj"));

	// マップ
	wall_ = std::make_unique<Entity>();
	wall_->SetInstancedModel(context_->LoadInstancedModel("Resources/Block", "block.obj", 1000));

	floor_ = std::make_unique<Entity>();
	floor_->SetInstancedModel(context_->LoadInstancedModel("Resources/Floor", "floor.obj", 1000));

	goal_ = std::make_unique<Entity>();
	goal_->SetModel(context_->LoadModel("Resources/Tiles", "sphere.obj"));


	mapTile_ = new MapTile();
	mapTile_->Initialize(wall_.get(), floor_.get(),goal_.get(),context_);
	mapTile_->LoadCSV("Resources/mapData.csv");

	// マップ判定
	mapCheck_ = new MapCheck();
	mapCheck_->Initialize(mapTile_->GetMap(), mapTile_->GetTileSize());

	// 武器マネージャー
	weaponManager_ = new WeaponManager();
	weaponManager_->Initilaize(context_);

	// アイテムマネージャー
	itemManager_ = new ItemManager();
	itemManager_->Initialize(weaponManager_,context_);

	// 当たり判定
	collisionChecker_ = new CollisionChecker();

	// エフェクト
	effectManager_ = new EffectManager();
	effectManager_->Initialize(context_);

	// プレイヤー
	player_ = new Player();
	player_->Initialize(playerModel_.get(),context_);

	// 敵
	enemyManager_ = new EnemyManager();
	enemyManager_->Initialize(context_);

	// 弾
	bulletManager_ = new BulletManager();
	bulletManager_->Initialize(effectManager_);

	// 天球
	skydome_ = std::make_unique<Entity>();
	skydome_->SetModel(context_->LoadModel("Resources/Skydome", "skydome.obj", false));
	camera_->transform_.translate = player_->GetTransform().translate + Vector3{ 0,0,-cameraDistance_ };

	// フェード
	fade_ = std::make_unique<Entity>();
	fade_->SetSprite(context_->LoadSprite("resources/white1x1.png"));
	fade_->GetSprite()->SetSize(context_->GetWindowSize() + Vector2{20,80});
	fade_->GetSprite()->SetColor({ 1.0f,1.0f,1.0f,0.0f });

	//enemyManager_->Spawn({ 25,0,12 }, context_, weaponManager_, 1);
	//enemyManager_->Spawn({ 16,0,18 }, context_, weaponManager_, 1);
	//enemyManager_->Spawn({ 5,0,24 }, context_, weaponManager_, 1);
	//enemyManager_->Spawn({ 17,0,25 }, context_, weaponManager_, 1);
	enemyManager_->Spawn({ 1,0,18 }, context_, weaponManager_, 1);

	itemManager_->Spawn({ 3,0,3 }, int(WeaponManager::WEAPON::FireBall));
	itemManager_->Spawn({ 4,0,12 }, int(WeaponManager::WEAPON::AssaultRifle));
}

void GameScene::Update() {
	// プレイヤー処理
	if (!isFadeOut_) {
		player_->Update(context_, mapCheck_, itemManager_, camera_, bulletManager_);
	}

	// ゲームオーバー
	if (player_->IsDead() && !isFadeOut_) {
		isFadeOut_ = true;
		fadeTimer_ = 0;
	}

	// ゴール判定
	Vector2 pos = { player_->GetTransform().translate.x,player_->GetTransform().translate.z };
	if (mapCheck_->IsGoal(pos, player_->GetRadius()) && !isFadeOut_) {
		isFadeOut_ = true;
		fadeTimer_ = 0;
	}

	// カメラ追従
	camera_->transform_.translate = player_->GetTransform().translate + Vector3{ 0,50,0 };
	camera_->UpdateCamera(context_, *debugCamera_);
	debugCamera_->Update();

	// 敵
	enemyManager_->Update(context_, mapCheck_, player_, bulletManager_);

	// 弾の処理
	bulletManager_->Update(mapCheck_);
	for (const auto& bullet : bulletManager_->GetBullets()) {

		// 当たり判定
		collisionChecker_->Check(player_, bullet);

		for (auto enemy : enemyManager_->GetEnemies()) {
			collisionChecker_->Check(enemy, bullet);
		}
	}

	// アイテム
	itemManager_->Update(player_);

	if (isFadeIn_) {
		fadeTimer_++;
		fade_->GetSprite()->SetColor({ 1.0f,1.0f,1.0f,1.0f - (float)fadeTimer_ / (float)kMaxFadeinTimer_ });
		if (fadeTimer_ >= kMaxFadeinTimer_) {
			isFadeIn_ = false;
			fadeTimer_ = kMaxFadeoutTimer_;
		}
	} else if (isFadeOut_) {
		fadeTimer_++;
		fade_->GetSprite()->SetColor({ 1.0f,1.0f,1.0f,(float)fadeTimer_ / (float)kMaxFadeoutTimer_ });
		if (fadeTimer_ >= kMaxFadeoutTimer_) {
			isFadeOut_ = false;
			finished_ = true;
		}
	}

	// マップ
	mapTile_->Update(context_);
}

void GameScene::Draw() {
	context_->DrawEntity(*skydome_, *camera_);
	mapTile_->Draw(context_, camera_);
	player_->Draw(context_, camera_);
	enemyManager_->Draw(context_, camera_);
	bulletManager_->Draw(context_, camera_);
	itemManager_->Draw(camera_);
	effectManager_->Draw(context_, camera_);
	context_->DrawEntity(*fade_,*camera_);

#ifdef USE_IMGUI
	ImGui::Begin("Player Info");
	ImGui::DragFloat3("rot", &camera_->transform_.rotate.x, 0.1f);
	ImGui::DragFloat3("tra", &camera_->transform_.translate.x, 0.1f);
	ImGui::End();

	context_->DrawLightImGui();
#endif
}