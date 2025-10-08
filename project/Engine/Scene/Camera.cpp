#include "Camera.h"
#include "DebugCamera.h"

void Camera::UpdateCamera(Vector2 windowSize,DebugCamera& debugCamera) {
	// デバッグカメラがONならそちらを使う
	if (debugCamera.IsEnable()) {
		// デバッグカメラのビュー行列を使う
		viewMatrix_ = debugCamera.GetViewMatrix();
	} else {
		// 通常カメラのビュー
		viewMatrix_ = Inverse(MakeAffineMatrix(transform_));
	}
	
	projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, windowSize.x / windowSize.y, 0.1f, 100.0f);
}