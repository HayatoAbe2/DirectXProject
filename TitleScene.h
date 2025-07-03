#pragma once
#include "Input.h"
#include "Graphics.h"
#include "Model.h"
#include "Camera.h"
#include "DebugCamera.h"
#include "Fade.h"
class TitleScene {
	enum class Phase {
		kFadeIn,
		kMain,
		kFadeOut
	};
	Phase phase_ = Phase::kFadeIn;
public:

	~TitleScene();
	void Initialize(Input* input, HWND hwnd);
	void Update();
	void Draw();

	bool isFinished() const { return finished_; };
private:
	// シーンの終了
	bool finished_ = false;

	// 描画
	Graphics graphics_;

	// 入力
	Input* input_;

	// モデル
	Model* model_;

	// カメラ
	Camera camera_;

	// デバッグカメラ
	bool isDebugCameraActive_ = false;
	DebugCamera* debugCamera_ = nullptr;

	float theta_;

	Fade* fade_ = nullptr;
};