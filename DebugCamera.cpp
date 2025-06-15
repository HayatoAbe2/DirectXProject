#include "DebugCamera.h"
#include "numbers"
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

void DebugCamera::Initialize() {

}

void DebugCamera::Update(const BYTE* preKey, const BYTE* key, const DIMOUSESTATE& mouse) {
	if (!preKey[DIK_RSHIFT] && key[DIK_RSHIFT]) {
		// デバッグカメラの切り替え
		isEnable_ = !isEnable_;
	}

	if (isEnable_) {
		ControlCamera(key,mouse);
		UpdateView();
	}
}

void DebugCamera::ControlCamera(const BYTE* key,const DIMOUSESTATE& mouse) {
	// 前進・後退
	if (key[DIK_UP]) {
		Vector3 move = { 0,0,kMoveSpeed_ };
		move = TransformVector(move, rotateMatrix_);
		translation_ = Add(translation_, move);
	}
	if (key[DIK_DOWN]) {
		Vector3 move = { 0,0,-kMoveSpeed_ };
		move = TransformVector(move, rotateMatrix_);
		translation_ = Add(translation_, move);
	}

	// 左右移動
	if (key[DIK_LEFT]) {
		Vector3 move = { -kMoveSpeed_,0,0 };
		move = TransformVector(move, rotateMatrix_);
		translation_ = Add(translation_, move);
	}
	if (key[DIK_RIGHT]) {
		Vector3 move = { kMoveSpeed_,0,0 };
		move = TransformVector(move, rotateMatrix_);
		translation_ = Add(translation_, move);
	}

	if (key[DIK_PGUP]) {
        Vector3 move = { 0, kMoveSpeed_, 0 };  // 上方向へ
        translation_ = Add(translation_, move);
    }
    if (key[DIK_PGDN]) {
        Vector3 move = { 0, -kMoveSpeed_, 0 }; // 下方向へ
        translation_ = Add(translation_, move);
    }

	if (mouse.rgbButtons[1] & 0x80) {  // 右クリック押されている
		// マウスの移動量に回転速度を掛ける
		float deltaYaw = -mouse.lX * kRotateSpeed_;   // マウスXでY軸回転（左右）
		float deltaPitch = -mouse.lY * kRotateSpeed_; // マウスYでX軸回転（上下）

		rotation_.y += deltaYaw;
		rotation_.x += deltaPitch;
	}
}

void DebugCamera::UpdateView() {
	// 角度から回転行列を計算
	rotateMatrix_ = Multiply(Multiply(
		MakeRotateXMatrix(rotation_.x),
		MakeRotateYMatrix(rotation_.y)),
		MakeRotateZMatrix(rotation_.z));

	// 座標から平行移動行列を計算する
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translation_);

	// ワールド行列を計算
	Matrix4x4 worldMatrix = Multiply(rotateMatrix_, translateMatrix);

	// ワールド行列の逆行列をビュー行列に代入
	viewMatrix_ = Inverse(worldMatrix);
}





