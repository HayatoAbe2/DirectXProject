#include "Skydome.h"
#include "Transform.h"

void Skydome::Initialize(Model* model) {

	model_ = model;

	// ワールドトランスフォームの初期化
	worldTransform_ = { {1.0f,1.0f,1.0f} };
}

void Skydome::Update() { 
	
}