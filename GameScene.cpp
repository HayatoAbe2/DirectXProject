#include "GameScene.h"

GameScene::~GameScene() {
	delete playerModel_;
	delete enemyModel_;
	delete blockModel_;
	delete skydomeModel_;
	delete deathParticleModel_;
	delete player_;
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();
	delete deathParticles_;
	delete debugCamera_;
	delete mapChipField_;

	for (std::vector<Transform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (Transform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}

void GameScene::Initialize(Input *input,HWND hwnd) {
	input_ = input;

	// 描画機能の初期化
	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;
	graphics_.Initialize(kClientWidth, kClientHeight, hwnd);

	// 3Dモデルの生成
	playerModel_ = Model::LoadObjFile("Resources/player", "player.obj",graphics_.GetDevice(),graphics_);
	enemyModel_ = Model::LoadObjFile("Resources/player", "player.obj",graphics_.GetDevice(),graphics_); // あとで変更
	blockModel_ = Model::LoadObjFile("Resources/block", "block.obj",graphics_.GetDevice(),graphics_);
	skydomeModel_ = Model::LoadObjFile("Resources/skydome", "skydome.obj",graphics_.GetDevice(),graphics_);
	deathParticleModel_ = Model::LoadObjFile("Resources/sphere", "sphere.obj",graphics_.GetDevice(),graphics_);

	phase_ = Phase::kPlay;

	// 天球の生成
	skydome_ = new Skydome();
	// 天球の初期化
	skydome_->Initialize(skydomeModel_);

	// マップチップフィールド
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("./Resources/mapData.csv");

	// ブロックの初期化
	GenerateBlocks();

	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(2, 17);

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(playerModel_, playerPosition,input_);
	player_->SetMapChipField(mapChipField_);

	// 敵キャラの生成
	for (uint32_t i = 0; i < enemyCount_; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(10 + i, 18);
		newEnemy->Initialize(enemyModel_, enemyPosition);
		enemies_.push_back(newEnemy);
	}

	// やられたときのパーティクル初期化
	deathParticles_ = new DeathParticles();
	deathParticles_->Initialize(deathParticleModel_, playerPosition);

	// カメラの初期化
	camera_ = { {1.0f,1.0f,1.0f} };

	// カメラコントローラの初期化
	cameraController_ = new CameraController();
	cameraController_->SetMovableArea_({ 0, 100, 0, 100 });
	cameraController_->Initialize(&camera_);
	cameraController_->SetTarget(player_);
	cameraController_->Reset();

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera();
}

void GameScene::Update() {

	switch (phase_) {
	case Phase::kPlay:

		// 天球の更新
		skydome_->Update();

		// 自キャラの更新
		player_->Update();

		// 敵キャラの更新
		for (Enemy* enemy : enemies_) {

			if (enemy) { // いなかったら処理しない
				enemy->Update();
			}
		}

		// カメラコントローラ更新
		cameraController_->Update();

#ifdef _DEBUG
		// デバッグカメラの更新
		debugCamera_->Update(input_);
#endif

		// ブロックの更新
		for (std::vector<Transform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (Transform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock) {
					continue;
				}

				Matrix4x4 affineMatrix =
					Multiply(Multiply(
						// 拡大縮小行列
						MakeScaleMatrix(worldTransformBlock->scale),
						// 回転行列
						Multiply(Multiply(MakeRotateXMatrix(worldTransformBlock->rotate.x), MakeRotateYMatrix(worldTransformBlock->rotate.y)), MakeRotateZMatrix(worldTransformBlock->rotate.z))),
						// 平行移動行列
						MakeTranslateMatrix(worldTransformBlock->translate));
			}
		}

		CheckAllCollisions();

		break;
	case Phase::kDeath:

		// 天球の更新
		skydome_->Update();

		// 敵キャラの更新
		for (Enemy* enemy : enemies_) {

			if (enemy) { // いなかったら処理しない
				enemy->Update();
			}
		}

		// パーティクルの更新
		if (deathParticles_) {
			deathParticles_->Update();
		}

		// カメラの処理
		camera_.UpdateCamera(graphics_,*debugCamera_);

		// ブロックの更新
		for (std::vector<Transform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (Transform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock) {
					continue;
				}

				Matrix4x4 affineMatrix =
					Multiply(Multiply(
						// 拡大縮小行列
						MakeScaleMatrix(worldTransformBlock->scale),
						// 回転行列
						Multiply(Multiply(MakeRotateXMatrix(worldTransformBlock->rotate.x), MakeRotateYMatrix(worldTransformBlock->rotate.y)), MakeRotateZMatrix(worldTransformBlock->rotate.z))),
						// 平行移動行列
						MakeTranslateMatrix(worldTransformBlock->translate));
			}
		}

		// プレイヤー、カメラコントローラの更新はしない

		break;
	}

}

void GameScene::Draw() {
	graphics_.BeginFrame();
	
	// カメラの更新
	camera_.UpdateCamera(graphics_, *debugCamera_);

	// 自キャラの描画
	player_->Draw(camera_,graphics_);

	// 敵キャラの描画
	for (Enemy* enemy : enemies_) {
		if (enemy) {
			enemy->Draw(camera_,graphics_);
		}
	}

	// パーティクル描画
	if (deathParticles_) {
		deathParticles_->Draw(camera_, graphics_);
	}

	// ブロックの描画
	for (std::vector<Transform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (Transform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
			blockModel_->SetTransform(*worldTransformBlock);
			blockModel_->UpdateModel(camera_);
			blockModel_->Draw(graphics_);
		}
	}

	// 天球の描画
	skydome_->Draw(camera_, graphics_);

	graphics_.EndFrame();
}

void GameScene::GenerateBlocks() {
	// 要素数
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を変更する
	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		// 一列の要素数
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	// キューブの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				Transform* worldTransform = new Transform;
				worldTransform->scale ={1.0f, 1.0f, 1.0f};
				worldTransform->rotate = { 0.0f, 0.0f, 0.0f };
				worldTransform->translate = { 0.0f, 0.0f, 0.0f };

				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translate = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

void GameScene::CheckAllCollisions() {
	AABB aabb1, aabb2;
#pragma region プレイヤーと敵
	aabb1 = player_->GetAABB();
	for (Enemy* enemy : enemies_) {
		aabb2 = enemy->GetAABB();
		if (aabb1.CheckCollision(aabb1, aabb2)) {
			player_->OnCollision(enemy);
			enemy->OnCollision(player_);
		}
	}
#pragma endregion
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kPlay:
		if (player_->IsDead()) {
			// プレイヤーがやられたらフェーズを切り替える
			phase_ = Phase::kDeath;
			// 自キャラの座標を取得
			Vector3 deathParticlesPosition = player_->GetWorldTransform().translate;
			// パーティクルの位置を設定
			deathParticles_->Initialize(deathParticleModel_, deathParticlesPosition);
			return;
		}
	}
}