#define NOMINMAX
#include "CameraController.h"
#include "Player.h"
#include "Transform.h"

void CameraController::Initialize(Transform* camera) {
	// カメラの初期化
	camera_ = camera;
}

void CameraController::Update() {
	// 追従対象のワールドトランスフォームを参照
	const Transform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象の速度を取得
	const Vector3& targetVelocity = target_->GetVelocity();
	// 追従対象とオフセットと追従対象の速度からカメラの目標座標を計算
	Vector3 endTranslation;
	endTranslation.x = targetWorldTransform.translate.x + targetOffset_.x + targetVelocity.x * kVelocityBias;
	endTranslation.y = targetWorldTransform.translate.y + targetOffset_.y + targetVelocity.y * kVelocityBias;

	// 座標補間によりゆったり追従
	camera_->translate.x = Lerp(camera_->translate.x, endTranslation.x, kInterpolationRate);
	camera_->translate.y = Lerp(camera_->translate.y, endTranslation.y, kInterpolationRate);

	// 追従対象が画面外に出ないように補正
	camera_->translate.x = std::max(camera_->translate.x, target_->GetWorldTransform().translate.x + moveLimit.left);
	camera_->translate.x = std::min(camera_->translate.x, target_->GetWorldTransform().translate.x + moveLimit.right);
	camera_->translate.y = std::max(camera_->translate.y, target_->GetWorldTransform().translate.x + moveLimit.bottom);
	camera_->translate.y = std::min(camera_->translate.y, target_->GetWorldTransform().translate.x + moveLimit.top);
	// 移動範囲制限
	camera_->translate.x = std::max(camera_->translate.x, movableArea_.left);
	camera_->translate.x = std::min(camera_->translate.x, movableArea_.right);
	camera_->translate.y = std::max(camera_->translate.y, movableArea_.bottom);
	camera_->translate.y = std::min(camera_->translate.y, movableArea_.top);

}

void CameraController::Reset() {
	// 追従対象のワールドトランスフォームを参照
	const Transform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	camera_->translate.x = targetWorldTransform.translate.x + targetOffset_.x;
	camera_->translate.y = targetWorldTransform.translate.y + targetOffset_.y;
	camera_->translate.z = targetWorldTransform.translate.z + targetOffset_.z;
}
