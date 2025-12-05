#include "TitleScene.h"
#include "Entity.h"
#include "GameContext.h"
#include "Sprite.h"

TitleScene::~TitleScene() {
	delete camera_;
	delete debugCamera_;
}

void TitleScene::Initialize() {
	debugCamera_ = new DebugCamera;
	debugCamera_->Initialize(context_);
	camera_ = new Camera;
	camera_->transform_.rotate = { 1.0f,0,0 };

	// 天球
	skydome_ = std::make_unique<Entity>();
	skydome_->SetModel(context_->LoadModel("Resources/Skydome", "skydome.obj", false));

	// フェード
	fade_ = std::make_unique<Entity>();
	fade_->SetSprite(context_->LoadSprite("resources/white1x1.png"));
	fade_->GetSprite()->SetSize(context_->GetWindowSize() + Vector2{ 20,80 });
	fade_->GetSprite()->SetColor({ 1.0f,1.0f,1.0f,0.0f });

	control_ = std::make_unique<Entity>();
	control_->SetSprite(context_->LoadSprite("Resources/Control/gamestart.png"));
	control_->GetSprite()->SetSize({ 270,39 });
	control_->GetSprite()->SetPosition({ 640 - 135,710 - 200 });
}

void TitleScene::Update() {
	Vector3 rotate = skydome_->GetTransform().rotate;
	rotate.y += 0.015f;
	skydome_->SetRotate(rotate);

	camera_->UpdateCamera(context_, *debugCamera_);
	debugCamera_->Update();

	if (context_->IsTriggerLeft() && !isFadeIn_ && !isFadeOut_) {
		isFadeOut_ = true;
		fadeTimer_ = 0;
	}

	if (isFadeIn_) {
		fadeTimer_++;
		fade_->GetSprite()->SetColor({ 1.0f,1.0f,1.0f,1.0f - (float)fadeTimer_ / (float)kMaxFadeinTimer_ });
		if (fadeTimer_ >= kMaxFadeinTimer_) {
			isFadeIn_ = false;
			fadeTimer_ = kMaxFadeoutTimer_;
		}
	} else if (isFadeOut_) {
		fadeTimer_++;
		fade_->GetSprite()->SetColor({ 1.0f,1.0f,1.0f,(float)fadeTimer_ / (float)kMaxFadeoutTimer_ });
		if (fadeTimer_ >= kMaxFadeoutTimer_) {
			isFadeOut_ = false;

			// ゲームオーバーまたはクリア
			isScenefinished_ = true;
		}
	}
}

void TitleScene::Draw() {
	context_->DrawEntity(*skydome_, *camera_);
	context_->DrawEntity(*control_, *camera_);
	context_->DrawEntity(*fade_, *camera_);
}
