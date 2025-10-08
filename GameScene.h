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
	void Initialize() override;

	// 更新
	void Update() override;

	// 描画
	void Draw() override;

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// 3Dモデル
	Model* playerModel_ = nullptr;
	
	// カメラ
	Camera* camera_ = nullptr;

	// デバッグカメラ
	bool isDebugCameraActive_ = false;
	DebugCamera* debugCamera_ = nullptr;
};
