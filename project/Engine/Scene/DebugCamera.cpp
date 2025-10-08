#include "DebugCamera.h"
#include "numbers"
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

void DebugCamera::Initialize() {
	translation_ = { 0, 0, -50 };
	matRot_ = MakeIdentity4x4();
	target_ = { 0, 0, 0 };
	distance_ = 50.0f;
	yaw_ = 0.0f;
	pitch_ = 0.0f;
	viewMatrix_ = MakeIdentity4x4();
	isEnable_ = false;
}

void DebugCamera::Update(Input* input) {
	if (input->IsTrigger(DIK_RSHIFT)) {
		// デバッグカメラの切り替え
		isEnable_ = !isEnable_;
	}

	if (isEnable_) {
		ControlCamera(input);

		UpdateView();
	}
}

void DebugCamera::ControlCamera(Input* input) { // 球面座標系での移動
	// カメラ前方向（ローカルZ+）をワールドに
	Vector3 forward = { 0,0,1 };
	Vector3 right = { 1,0,0 };
	Vector3 up = { 0,1,0 };

	// shift+マウスホイール押し込み中,ドラッグで視点移動
	if (input->IsPress(DIK_LSHIFT) && input->isClickWheel()) {

		float moveX = input->GetMouseMove().x * kMoveSpeed_;
		float moveY = input->GetMouseMove().y * kMoveSpeed_;

		target_ = target_ + moveX * -right;
		target_ = target_+ moveY * up;

	} else {

		// マウスホイール押し込み中,ドラッグで視点回転
		if (input->isClickWheel()) {
			// マウスの移動量に回転速度を掛ける
			float deltaYaw = input->GetMouseMove().x * kRotateSpeed_;   // マウスXでY軸回転（左右）
			float deltaPitch = input->GetMouseMove().y * kRotateSpeed_; // マウスYでX軸回転（上下）

			Matrix4x4 matRotDelta = MakeIdentity4x4();
			matRotDelta = Multiply(MakeRotateYMatrix(deltaYaw), matRotDelta);
			matRotDelta = Multiply(matRotDelta, MakeRotateXMatrix(deltaPitch));

			matRot_ = Multiply(matRotDelta, matRot_);
		}
	}

	// マウスホイールでズームイン・ズームアウト
	float moveZ = input->GetMouseMove().z * kMoveSpeed_;
	distance_ += -moveZ;

	// カメラは注視点から後ろ向きにdistance_移動した位置
	Vector3 back;
	back = { 0,0,-distance_ };
	translation_ = target_ + back;
}

void DebugCamera::UpdateView() {

	// 座標から平行移動行列を計算する
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translation_);

	// ワールド行列を計算
	Matrix4x4 worldMatrix = Multiply(translateMatrix, matRot_);

	// ワールド行列の逆行列をビュー行列に代入
	viewMatrix_ = Inverse(worldMatrix);
}