#include "Skydome.h"
#include "Transform.h"

void Skydome::Initialize(Model* model) {

	model_ = model;

	// ワールドトランスフォームの初期化
	worldTransform_ = { {1.0f,1.0f,1.0f} };
}

void Skydome::Update() { 
	
}

void Skydome::Draw(Camera& camera, Graphics& graphics) {
	model_->SetTransform(worldTransform_);
	model_->UpdateTransformation(camera);
	model_->Draw(graphics);
}