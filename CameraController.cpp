#define NOMINMAX
#include "CameraController.h"
#include "Player.h"
#include "Transform.h"
#include "Camera.h"

void CameraController::Initialize(Camera* camera) {
	// カメラの初期化
	camera_ = camera;
}

void CameraController::Update() {
	camera_->transform_.translate.y += 0.003f;
}

void CameraController::Reset() {
	// 追従対象のワールドトランスフォームを参照
	const Transform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	camera_->transform_.translate.x = targetWorldTransform.translate.x + targetOffset_.x;
	camera_->transform_.translate.y = targetWorldTransform.translate.y + targetOffset_.y;
	camera_->transform_.translate.z = targetWorldTransform.translate.z + targetOffset_.z;
}
