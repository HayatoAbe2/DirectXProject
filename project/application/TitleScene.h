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

class TitleScene : public BaseScene {
public:

	~TitleScene() override;

	// 初期化
	void Initialize() override;

	// 更新
	void Update() override;

	// 描画
	void Draw() override;

private:
	// 3Dモデル
	std::unique_ptr<Entity> skydome_ = nullptr;
	std::unique_ptr<Entity> fade_ = nullptr;

	// 操作
	std::unique_ptr<Entity> control_ = nullptr;

	// カメラ
	Camera* camera_ = nullptr;
	float cameraDistance_ = 20.0f;

	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;

	// フェード
	bool isFadeIn_ = true;
	bool isFadeOut_ = false;
	int fadeTimer_ = 0;
	const int kMaxFadeinTimer_ = 30;
	const int kMaxFadeoutTimer_ = 30;
};
