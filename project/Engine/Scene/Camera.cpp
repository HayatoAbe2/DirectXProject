#include "Camera.h"
#include "DebugCamera.h"
#include "GameContext.h"

void Camera::UpdateCamera(GameContext* context, DebugCamera& debugCamera) {
	// デバッグカメラがONならそちらを使う
	if (debugCamera.IsEnable()) {
		// デバッグカメラのビュー行列を使う
		viewMatrix_ = debugCamera.GetViewMatrix();
	} else {
		// カメラシェイク
		if (shakeFrame_ > 0) {
			shakeFrame_--;

			float amp = amplitude_ * (float(shakeFrame_) / float(shakeEndFrame_));

			shake_ = {
				context->RandomFloat(-amp / 2.0f, amp / 2.0f),
				context->RandomFloat(-amp / 2.0f, amp / 2.0f),
				context->RandomFloat(-amp / 2.0f, amp / 2.0f),
			};
		} else {
			shake_ = {};
			amplitude_ = 0;
		}
		Transform shakedTransform = transform_;
		shakedTransform.translate += shake_;
		// 通常カメラのビュー
		viewMatrix_ = Inverse(MakeAffineMatrix(shakedTransform));
	}
	Vector2 windowSize = context->GetWindowSize();
	projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, windowSize.x / windowSize.y, 0.1f, 100.0f);
}

void Camera::StartShake(float amplitude, int frame) {
	amplitude_ = amplitude;
	shakeFrame_ = frame;
	shakeEndFrame_ = frame;
}
