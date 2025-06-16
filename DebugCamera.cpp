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
	// 今のカメラ方向行列を作る
	Matrix4x4 mYaw = MakeRotateYMatrix(yaw_);
	Matrix4x4 mPitch = MakeRotateXMatrix(pitch_);
	Matrix4x4 rot = Multiply(mPitch, mYaw);

	// カメラ前方向（ローカルZ+）をワールドに
	Vector3 forward = TransformVector({ 0,0,1 }, rot);
	Vector3 right = TransformVector({ 1,0,0 }, rot);

	if (input->IsPress(DIK_UP)) { target_ = Add(target_, Multiply(kMoveSpeed_, forward)); }
	if (input->IsPress(DIK_DOWN)) { target_ = Add(target_, Multiply(-kMoveSpeed_, forward)); }
	if (input->IsPress(DIK_LEFT)) { target_ = Add(target_, Multiply(-kMoveSpeed_, right)); }
	if (input->IsPress(DIK_RIGHT)) { target_ = Add(target_, Multiply(kMoveSpeed_, right)); }
	if (input->IsPress(DIK_PGUP)) { target_.y += kMoveSpeed_; }
	if (input->IsPress(DIK_PGDN)) { target_.y -= kMoveSpeed_; }

	if (input->isClickRight()) {  // 右クリック中
		// マウスの移動量に回転速度を掛ける
		float deltaYaw = input->GetMouseMove().x * kRotateSpeed_;   // マウスXでY軸回転（左右）
		float deltaPitch = input->GetMouseMove().y *kRotateSpeed_; // マウスYでX軸回転（上下）

		Matrix4x4 matRotDelta = MakeIdentity4x4();
		matRotDelta = Multiply(MakeRotateYMatrix(deltaYaw),matRotDelta);
		matRotDelta = Multiply(matRotDelta,MakeRotateXMatrix(deltaPitch));

		matRot_ = Multiply(matRotDelta,matRot_);
	}

	// カメラは注視点から後ろ向きにdistance_移動した位置
	Vector3 back = TransformVector({ 0,0,-distance_ }, rot);
	translation_ = Add(target_, back);
}

void DebugCamera::UpdateView() {

	// 座標から平行移動行列を計算する
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translation_);

	// ワールド行列を計算
	Matrix4x4 worldMatrix = Multiply(translateMatrix,matRot_ );

	// ワールド行列の逆行列をビュー行列に代入
	viewMatrix_ = Inverse(worldMatrix);
}