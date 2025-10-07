#pragma once
#include "Model.h"
#include "Sprite.h"
#include <list>
#include "Transform.h"
#include "DebugCamera.h"
#include "Camera.h"

#include "BaseScene.h"

class Input;
class Graphics;
class Camera;
class Audio;
// ゲームシーン
class GameScene : public BaseScene {
public:

	~GameScene() override;

	// 初期化
	void Initialize(Graphics* graphics) override;

	// 更新
	void Update(Input* input,Audio* audio) override;

	// 描画
	void Draw(Graphics *graphics) override;

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// 3Dモデル
	Model* PlayerModel_ = nullptr;
	
	// カメラ
	Camera* camera_ = nullptr;

	// デバッグカメラ
	bool isDebugCameraActive_ = false;
	DebugCamera* debugCamera_ = nullptr;
};
