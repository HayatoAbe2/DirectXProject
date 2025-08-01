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
// ゲームシーン
class GameScene : public BaseScene {
public:

	~GameScene() override;

	// 初期化
	void Initialize(Graphics* graphics) override;

	// 更新
	void Update(Input* input) override;

	// 描画
	void Draw(Graphics *graphics) override;

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// 3Dモデル
	Model* planeModel_ = nullptr;
	Sprite* checkerSprite_ = nullptr;
	Model* multiMeshModel_ = nullptr;
	Model* teapotModel_ = nullptr;
	Model* bunnyModel_ = nullptr;

	// トランスフォーム
	Transform transformPlane_ = {1,1,1};
	Transform transformSphere_ = { 1,1,1 };
	Transform transformMultiMesh_ = { 1,1,1 };
	Transform transformTeapot_ = { 1,1,1 };
	Transform transformBunny_ = { 1,1,1 };
	
	// カメラ
	Camera* camera_ = nullptr;

	// デバッグカメラ
	bool isDebugCameraActive_ = false;
	DebugCamera* debugCamera_ = nullptr;
};
