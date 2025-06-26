#include "Camera.h"
#include "DebugCamera.h"
#include "Graphics.h"

void Camera::UpdateCamera(Graphics &graphics,DebugCamera& debugCamera) {
	// デバッグカメラがONならそちらを使う
	if (debugCamera.IsEnable()) {
		// デバッグカメラのビュー行列を使う
		viewMatrix_ = debugCamera.GetViewMatrix();
	} else {
		// 通常カメラのビュー
		viewMatrix_ = Inverse(MakeAffineMatrix(transform_));
	}
	
	projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, float(graphics.GetWindowWidth()) / float(graphics.GetWindowHeight()), 0.1f, 100.0f);
}