#pragma once
#include "GameContext.h"
#include "BaseScene.h"

#include <memory>

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
	// モデル
	std::unique_ptr<Model> skydome_ = nullptr;

	// スプライト
	std::unique_ptr<Sprite> fade_ = nullptr;
	std::unique_ptr<Sprite> control_ = nullptr;
	std::unique_ptr<Sprite> logo_ = nullptr;

	// カメラ
	std::unique_ptr<Camera> camera_ = nullptr;
	float cameraDistance_ = 20.0f;

	// デバッグカメラ
	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;

	// フェード
	bool isFadeIn_ = true;
	bool isFadeOut_ = false;
	int fadeTimer_ = 0;
	const int kMaxFadeinTimer_ = 30;
	const int kMaxFadeoutTimer_ = 30;
};
