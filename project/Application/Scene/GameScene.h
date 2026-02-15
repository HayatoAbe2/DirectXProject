#pragma once
#include "Scene/BaseScene.h"

// ゲームシーン
class GameScene : public BaseScene {
public:

	~GameScene() override;

	// 初期化
	void Initialize() override;

	// 更新
	void Update() override;

	// 描画
	void Draw() override;

private:
	// 3Dモデル
	std::unique_ptr<Model> playerModel_ = nullptr;

	// カメラ
	std::unique_ptr<Camera> camera_ = nullptr;
	float cameraDistance_ = 20.0f;

	// デバッグカメラ
	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;
};
