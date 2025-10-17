#pragma once
#include "Model.h"
#include "Sprite.h"
#include <list>
#include "Transform.h"
#include "DebugCamera.h"
#include "Camera.h"
#include "BaseScene.h"

class Renderer;
class Camera;
class Player;
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
	Model* playerModel_ = nullptr;
	Model* planeModel_ = nullptr;

	// プレイヤー
	Player* player_ = nullptr;

	// カメラ
	Camera* camera_ = nullptr;
	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;

	int numPlaneInstance_ = 10;
	Transform planeTransforms_[10]{};
};
