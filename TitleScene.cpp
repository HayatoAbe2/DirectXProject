#include "TitleScene.h"
#include "numbers"

TitleScene::~TitleScene() {
	graphics_.Finalize();
	delete model_;
	delete fade_;
}

void TitleScene::Initialize(Input* input, HWND hwnd) {
	input_ = input;
	debugCamera_ = new DebugCamera;
	debugCamera_->Initialize();

	// 描画機能の初期化
	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;
	graphics_.Initialize(kClientWidth, kClientHeight, hwnd);
	camera_ = { {1.0f,1.0f,1.0f} };
	model_ = Model::LoadObjFile("Resources/player", "player.obj", graphics_.GetDevice(), graphics_);

	fade_ = new Fade();
	fade_->Initialize(hwnd,&graphics_,&camera_);
	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

void TitleScene::Update() {
	// 回転
	theta_ += float(std::numbers::pi) / 60.0f;
	model_->SetTransform({ {1,1,1},{0,theta_,0},{0,0,0} });

	fade_->Update();
	switch (phase_) {
	case Phase::kFadeIn:
		// フェードイン終了後
		if (fade_->IsFinish()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kMain:
		// シーンの終了
		if (input_->IsPress(DIK_SPACE)) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f); // フェードアウト開始
		}
		break;
	case Phase::kFadeOut:
		if (fade_->IsFinish()) {
			finished_ = true;
		}
		break;
	}

#ifdef _DEBUG
		// デバッグカメラの更新
		debugCamera_->Update(input_);
#endif

		fade_->Update();
	
}

void TitleScene::Draw() {
	graphics_.BeginFrame();

	camera_.transform_ = { {1,1,1},{0,0,0},{0,2,-20} };
	camera_.UpdateCamera(graphics_, *debugCamera_);
	model_->UpdateTransformation(camera_);
	model_->Draw(graphics_);
	fade_->Draw();

	graphics_.EndFrame();
}
