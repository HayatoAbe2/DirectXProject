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
#include "Camera.h"

#include "BaseScene.h"

class Input;
class Graphics;
class Camera;
// ゲームシーン
class GameScene : public BaseScene {
public:

	~GameScene();

	// 初期化
	void Initialize() override;

	// 更新
	void Update(Input* input) override;

	// 描画
	void Draw(Graphics *graphics) override;

	void GenerateBlocks();

	// 当たり判定
	void CheckAllCollisions();

	// フェーズの切り替え
	void ChangePhase();

	// 終了チェック
	bool isFinished() const { return finished_; };

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// 3Dモデル
	Model* playerModel_ = nullptr;
	Model* enemyModel_ = nullptr;
	Model* blockModel_ = nullptr;
	Model* skydomeModel_ = nullptr;
	Model* deathParticleModel_ = nullptr; 

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
		kFadeIn,
		kPlay,
		kDeath,
		kFadeOut,
	};
	Phase phase_; // 現在フェーズ

	// シーンの終了
	bool finished_ = false;
};
