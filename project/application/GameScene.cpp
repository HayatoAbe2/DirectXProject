#include "GameScene.h"
#include "MathUtils.h"
#include "GameContext.h"
#include "ParticleSystem.h"

#include <numbers>

#include "ImGuiManager.h"

GameScene::~GameScene() {
	context_->SoundUnload(L"Resources/Sounds/SE/explosion.mp3");
	context_->SoundUnload(L"Resources/Sounds/SE/shoot.mp3");
	context_->SoundUnload(L"Resources/Sounds/SE/fire.mp3");
	context_->SoundUnload(L"Resources/Sounds/SE/floorClear.mp3");
	context_->SoundUnload(L"Resources/Sounds/SE/fall.mp3");
	context_->SoundUnload(L"Resources/Sounds/SE/warp.mp3");
	context_->SoundUnload(L"Resources/Sounds/SE/hit.mp3");
}

void GameScene::Initialize() {
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(context_);
	camera_ = std::make_unique<Camera>();
	camera_->transform_.rotate = { 1.0f,0,0 };

	context_->SoundLoad(L"Resources/Sounds/SE/explosion.mp3");
	context_->SoundLoad(L"Resources/Sounds/SE/shoot.mp3");
	context_->SoundLoad(L"Resources/Sounds/SE/fire.mp3");
	context_->SoundLoad(L"Resources/Sounds/SE/floorClear.mp3");
	context_->SoundLoad(L"Resources/Sounds/SE/fall.mp3");
	context_->SoundLoad(L"Resources/Sounds/SE/warp.mp3");
	context_->SoundLoad(L"Resources/Sounds/SE/hit.mp3");

	playerModel_ = context_->LoadModel("Resources/Player", "player.obj");

	// マップ
	wall_ = context_->LoadInstancedModel("Resources/Block", "block.obj", 1000);
	floor_ = context_->LoadInstancedModel("Resources/Floor", "floor.obj", 1000);
	goal_ = context_->LoadModel("Resources/Tiles", "sphere.obj");

	mapTile_ = std::make_unique<MapTile>();
	mapTile_->Initialize(std::move(wall_), std::move(floor_),std::move(goal_),context_);

	// 武器マネージャー
	weaponManager_ = std::make_unique<WeaponManager>();
	weaponManager_->Initilaize(context_);

	// アイテムマネージャー
	itemManager_ = std::make_unique<ItemManager>();
	itemManager_->Initialize(weaponManager_.get(), context_);

	// エフェクト
	effectManager_ = std::make_unique<EffectManager>();
	effectManager_->Initialize(context_);

	// 当たり判定
	collisionChecker_ = std::make_unique<CollisionChecker>();
	collisionChecker_->Inititalize(effectManager_.get(), context_);

	// プレイヤー
	player_ = std::make_unique<Player>();
	player_->Initialize(std::move(playerModel_),context_);

	// 敵
	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize(context_);

	// 弾
	bulletManager_ = std::make_unique<BulletManager>();
	
	// 天球
	skydome_ = std::make_unique<Model>();
	skydome_ = context_->LoadModel("Resources/Skydome", "skydome.obj", false);

	// 雲
	cloud_ = std::make_unique<Model>();
	cloud_ = context_->LoadModel("Resources/Cloud", "Cloud.obj", false);
	cloud_->SetTranslate({ 0,-20,0 });
	MaterialData data = cloud_->GetMaterial(0)->GetData();
	data.color.w = 1.0f;
	cloud_->GetMaterial(0)->SetData(data);

	camera_->transform_.translate = player_->GetTransform().translate + Vector3{ 0,0,-cameraDistance_ };

	// フェード
	fade_ = std::make_unique<Sprite>();
	fade_ = context_->LoadSprite("resources/white1x1.png");
	fade_->SetSize(context_->GetWindowSize() + Vector2{20,80});
	fade_->SetColor({ 1.0f,1.0f,1.0f,0.0f });

	// マップ判定
	mapCheck_ = std::make_unique<MapCheck>();

	Reset();
}

void GameScene::Update() {
	MaterialData data = cloud_->GetMaterial(0)->GetData();
	data.uvTransform.m[3][1] += 0.001f;
	cloud_->GetMaterial(0)->SetData(data);

	// プレイヤー処理
	if (!isFadeOut_) {
		player_->Update(context_, mapCheck_.get(), itemManager_.get(), camera_.get(), bulletManager_.get());
	}

	// ゲームオーバー
	if (player_->IsDead() && !isFadeOut_) {
		isFadeOut_ = true;
		fadeTimer_ = 0;
	}

	// ゴール判定
	Vector2 pos = { player_->GetTransform().translate.x,player_->GetTransform().translate.z };
	if (mapCheck_->IsGoal(pos, player_->GetRadius(),enemyManager_->GetEnemies().size() == 0) && !isFadeOut_) {
		isFadeOut_ = true;
		fadeTimer_ = 0;
		context_->SoundPlay(L"Resources/Sounds/SE/warp.mp3", false);
	}

	// カメラ追従
	camera_->transform_.translate = player_->GetTransform().translate + Vector3{ 0,30,-19 };
	camera_->UpdateCamera(context_, *debugCamera_);
	debugCamera_->Update();

	// 敵
	enemyManager_->Update(context_, mapCheck_.get(), player_.get(), bulletManager_.get());

	// 弾の処理
	bulletManager_->Update(mapCheck_.get());
	for (const auto& bullet : bulletManager_->GetBullets()) {

		// 当たり判定
		collisionChecker_->Check(player_.get(), bullet, camera_.get());

		for (auto enemy : enemyManager_->GetEnemies()) {
			collisionChecker_->Check(enemy, bullet, camera_.get());
		}
	}

	// アイテム
	itemManager_->Update(player_.get());

	if (isFadeIn_) {
		fadeTimer_++;
		fade_->SetColor({ 1.0f,1.0f,1.0f,1.0f - (float)fadeTimer_ / (float)kMaxFadeinTimer_ });
		if (fadeTimer_ >= kMaxFadeinTimer_) {
			isFadeIn_ = false;
			fadeTimer_ = 0;
		}
	} else if (isFadeOut_) {
		fadeTimer_++;
		fade_->SetColor({ 1.0f,1.0f,1.0f,(float)fadeTimer_ / (float)kMaxFadeoutTimer_ });
		if (fadeTimer_ >= kMaxFadeoutTimer_) {
			isFadeOut_ = false;

			if (player_->IsDead() || currentFloor_ == 3) {
				// ゲームオーバーまたはクリア
				isScenefinished_ = true;
			} else {
				fadeTimer_ = 0;
				isFadeIn_ = true;
				// 次のフロア
				currentFloor_++;
				Reset();
			}
		}
	}

	// マップ
	mapTile_->Update(enemyManager_->GetEnemies().size() == 0);

	effectManager_->Update();
}

void GameScene::Draw() {
	context_->DrawModel(skydome_.get(), camera_.get());
	context_->DrawModel(cloud_.get(), camera_.get(), BlendMode::Add);
	mapTile_->Draw(camera_.get());
	player_->Draw(context_, camera_.get());
	enemyManager_->Draw(context_, camera_.get());
	bulletManager_->Draw(context_, camera_.get());
	itemManager_->Draw(camera_.get());
	effectManager_->Draw(context_, camera_.get());
	context_->DrawSprite(fade_.get());

#ifdef USE_IMGUI
	ImGui::Begin("Player Info");
	ImGui::DragFloat3("rot", &camera_->transform_.rotate.x, 0.1f);
	ImGui::DragFloat3("tra", &camera_->transform_.translate.x, 0.1f);
	ImGui::End();

	context_->DrawLightImGui();
#endif
}

void GameScene::Reset() {
	enemyManager_->Reset();
	itemManager_->Reset();
	bulletManager_->Reset();

	int floorType = 0;
	std::string path;
	switch (currentFloor_) {
	case 1:
		/*floorType = context_->RandomInt(1, 3);
		path = "Resources/MapData/Floor" + std::to_string(floorType) + ".csv";
		mapTile_->LoadCSV(path);*/

		// マップを再構築
		mapTile_->LoadCSV("Resources/MapData/Floor1.csv");

		// プレイヤー位置
		player_->SetTransform({ { 1,1,1 }, { 0,0,0 }, {3,0,3} });

		// その階の敵とアイテム
		enemyManager_->Spawn({ 25,0,12 }, context_, weaponManager_.get(), 1);
		enemyManager_->Spawn({ 16,0,18 }, context_, weaponManager_.get(), 2);
		enemyManager_->Spawn({ 3,0,18 }, context_, weaponManager_.get(), 1);
		enemyManager_->Spawn({ 5,0,24 }, context_, weaponManager_.get(), 1);
		itemManager_->Spawn({ 10,0,5 }, int(WeaponManager::WEAPON::Pistol));
		itemManager_->Spawn({ 26,0,12 }, int(WeaponManager::WEAPON::AssaultRifle));
		break;

	case 2:
		// マップを再構築
		mapTile_->LoadCSV("Resources/MapData/Floor2.csv");

		// プレイヤー位置
		player_->SetTransform({ { 1,1,1 }, { 0,0,0 }, {3,0,27} });

		// その階の敵とアイテム
		enemyManager_->Spawn({ 25,0,12 }, context_, weaponManager_.get(), 1);
		enemyManager_->Spawn({ 16,0,18 }, context_, weaponManager_.get(), 1);
		enemyManager_->Spawn({ 12,0,21 }, context_, weaponManager_.get(), 2);
		enemyManager_->Spawn({ 24,0,25 }, context_, weaponManager_.get(), 2);
		enemyManager_->Spawn({ 4,0,18 }, context_, weaponManager_.get(), 2);
		itemManager_->Spawn({ 27,0,20 }, int(WeaponManager::WEAPON::Shotgun));
		itemManager_->Spawn({ 26,0,5 }, int(WeaponManager::WEAPON::FireBall));
		break;

	case 3:
		// マップを再構築
		mapTile_->LoadCSV("Resources/MapData/Floor3.csv");

		// プレイヤー位置
		player_->SetTransform({ { 1,1,1 }, { 0,0,0 }, {3,0,3} });

		// その階の敵とアイテム
		enemyManager_->Spawn({ 25,0,12 }, context_, weaponManager_.get(), 3);
		break;
	}

	mapCheck_->Initialize(mapTile_->GetMap(), mapTile_->GetTileSize());
}
