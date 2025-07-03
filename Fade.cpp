#include "Fade.h"
#include <algorithm>

Fade::~Fade() {
	delete sprite_;
}

void Fade::Initialize(HWND hwnd, Graphics* graphics, Camera* camera) {
	graphics_ = graphics;
	camera_ = camera;
	sprite_ = Sprite::Initialize(graphics_, "Resources/white1x1.png", { float(graphics_->GetWindowWidth()), float(graphics_->GetWindowHeight()) });
}

void Fade::Update() {
	switch (status_) {
	case Status::None:
		break;
	case Status::FadeIn:
		counter_ += 1.0f / 60.0f;
		counter_ = (std::min)(counter_, duration_);
		sprite_->SetColor({ 0,0,0,1.0f - std::clamp(counter_ / duration_,0.0f,1.0f) });
		break;
	case Status::FadeOut:
		counter_ += 1.0f / 60.0f;
		counter_ = (std::min)(counter_, duration_);
		sprite_->SetColor({ 0,0,0,std::clamp(counter_ / duration_,0.0f,1.0f) });
		break;
	}
}

void Fade::Draw() {
	if (status_ == Status::None) { return; }
	sprite_->UpdateTransform(camera_, float(graphics_->GetWindowWidth()), float(graphics_->GetWindowHeight()));
	sprite_->Draw(*graphics_,sprite_->GetColor());
}

void Fade::Start(Status status, float duration) {
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

void Fade::Stop() {
	status_ = Status::None;
}

bool Fade::IsFinish() const {
	switch (status_) {
	case Status::FadeIn:
	case Status::FadeOut:
		return counter_ >= duration_;
	}
	return true;
}
