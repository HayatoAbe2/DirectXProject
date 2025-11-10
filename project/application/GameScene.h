#pragma once
#include "Transform.h"
#include "DebugCamera.h"
#include "Camera.h"
#include "BaseScene.h"
#include <list>
#include <memory>
#include <vector>
#include <random>

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
	std::unique_ptr<Entity> particle_ = nullptr;

	// プレイヤー
	Player* player_ = nullptr;

	// カメラ
	Camera* camera_ = nullptr;
	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;
	int numPlaneInstance_ = 10;
	std::vector<Transform> planeTransforms_;

	std::random_device randomDevice_;
	std::mt19937 randomEngine_;
};
