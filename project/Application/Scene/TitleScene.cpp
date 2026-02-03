#include "TitleScene.h"
#include "Sprite.h"

TitleScene::~TitleScene() {
}

void TitleScene::Initialize() {
	context_->SoundLoad(L"Resources/Sounds/SE/press.mp3");

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(context_);
	camera_ = std::make_unique<Camera>();
	camera_->transform_.rotate = { 0,0,0 };

	// 天球
	skydome_ = context_->LoadModel("Resources/Skydome", "skydome.obj", false);
	
	// フェード
	fade_ = context_->LoadSprite("resources/white1x1.png");
	fade_->SetSize(context_->GetWindowSize() + Vector2{ 20,80 });
	fade_->SetColor({ 1.0f,1.0f,1.0f,0.0f });

	control_ = context_->LoadSprite("Resources/Control/gamestart.png");
	control_->SetSize({ 270,39 });
	control_->SetPosition({ 640 - 135,710 - 200 });

	logo_ = context_->LoadSprite("Resources/Control/title.png");
	logo_->SetSize({ 610,150 });
	logo_->SetPosition({ 640 - 305,700 - 500 });
}

void TitleScene::Update() {
	Vector3 rotate = skydome_->GetTransform().rotate;
	skydome_->SetRotate(rotate);

	camera_->UpdateCamera(context_, *debugCamera_);
	debugCamera_->Update();

	if (context_->IsTriggerLeftClick() && !isFadeIn_ && !isFadeOut_) {
		isFadeOut_ = true;
		fadeTimer_ = 0;
		context_->SoundPlay(L"Resources/Sounds/SE/press.mp3", false);
	}

	if (isFadeIn_) {
		fadeTimer_++;
		fade_->SetColor({ 1.0f,1.0f,1.0f,1.0f - (float)fadeTimer_ / (float)kMaxFadeinTimer_ });
		if (fadeTimer_ >= kMaxFadeinTimer_) {
			isFadeIn_ = false;
			fadeTimer_ = kMaxFadeoutTimer_;
		}
	} else if (isFadeOut_) {
		fadeTimer_++;
		fade_->SetColor({ 1.0f,1.0f,1.0f,(float)fadeTimer_ / (float)kMaxFadeoutTimer_ });
		if (fadeTimer_ >= kMaxFadeoutTimer_) {
			isFadeOut_ = false;

			// ゲームオーバーまたはクリア
			isScenefinished_ = true;
		}
	}
}

void TitleScene::Draw() {
	context_->DrawModel(skydome_.get(), camera_.get());
	context_->DrawSprite(control_.get());
	context_->DrawSprite(logo_.get());
	context_->DrawSprite(fade_.get());
}
