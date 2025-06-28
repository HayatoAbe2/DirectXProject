#pragma once
#include "Player.h"
#include "Enemy.h"
#include "Skydome.h"
#include "MapChipField.h"
#include "CameraController.h"
#include "DeathParticles.h"
#include "Model.h"
#include <list>
#include "Transform.h"
#include "DebugCamera.h"
#include "Graphics.h"
#include "Input.h"
#include "Camera.h"

// ゲームシーン
class GameScene {
public:

	~GameScene();

	// 初期化
	void Initialize(Input* input, HWND hwnd);

	// 更新
	void Update();

	// 描画
	void Draw();

	void GenerateBlocks();

	// 当たり判定
	void CheckAllCollisions();

	// フェーズの切り替え
	void ChangePhase();

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// 3Dモデル
	Model* playerModel_ = nullptr;
	Model* enemyModel_ = nullptr;
	Model* blockModel_ = nullptr;
	Model* skydomeModel_ = nullptr;
	Model* deathParticleModel_ = nullptr; 

	// 描画情報
	Graphics graphics_;

	// カメラコントローラ
	CameraController* cameraController_;
	// カメラ
	Camera camera_;

	// 自キャラ
	Player* player_ = nullptr;

	// 敵キャラ
	std::list<Enemy*> enemies_;
	uint32_t enemyCount_ = 1;

	// パーティクル
	DeathParticles* deathParticles_ = nullptr;

	// ブロック(のワールドトランスフォーム)
	std::vector<std::vector<Transform*>> worldTransformBlocks_;

	// 天球
	Skydome* skydome_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipField_;

	// デバッグカメラ
	bool isDebugCameraActive_ = false;
	DebugCamera* debugCamera_ = nullptr;

	// ゲームのフェーズ
	enum class Phase {
		kPlay,
		kDeath,
	};
	Phase phase_; // 現在フェーズ

	// 入力
	Input* input_;
};
