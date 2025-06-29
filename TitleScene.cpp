#include "TitleScene.h"

TitleScene::~TitleScene() {
	graphics_.Finalize();
}

void TitleScene::Initialize(Input* input, HWND hwnd) {
	input_ = input;

	// 描画機能の初期化
	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;
	graphics_.Initialize(kClientWidth, kClientHeight, hwnd);

	//// カメラの初期化
	//camera_ = { {1.0f,1.0f,1.0f} };

}

void TitleScene::Update() {
	// シーンの終了
	if (input_->IsPress(DIK_SPACE)) {
		finished_ = true;
	}
}

void TitleScene::Draw() {
	graphics_.BeginFrame();


	graphics_.EndFrame();
}
