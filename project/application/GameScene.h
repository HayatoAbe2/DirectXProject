#pragma once
#include "Transform.h"
#include "DebugCamera.h"
#include "Camera.h"
#include "BaseScene.h"
#include <list>
#include <memory>

class Entity;
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
	std::unique_ptr<Entity> playerModel_ = nullptr;
	std::unique_ptr<Entity> planeModel_ = nullptr;
	std::unique_ptr<Entity> uvChecker_ = nullptr;
	std::unique_ptr<Entity> sprite_ = nullptr;

	// プレイヤー
	Player* player_ = nullptr;

	// カメラ
	Camera* camera_ = nullptr;
	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;
	int numPlaneInstance_ = 10;
	Transform planeTransforms_[10]{};
};
