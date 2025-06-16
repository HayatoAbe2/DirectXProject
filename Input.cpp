#include "Input.h"

Input::Input(WNDCLASS wc,HWND hwnd) {
	// オブジェクト生成
	DirectInput8Create(wc.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput_, nullptr);
	// キーボードデバイスの生成
	directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	// 入力データ形式のセット
	keyboard_->SetDataFormat(&c_dfDIKeyboard);
	// 排他制御レベルのセット
	keyboard_->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

	// マウスデバイス生成
	directInput_->CreateDevice(GUID_SysMouse, &mouse_, NULL);
	// 入力データ形式のセット
	mouse_->SetDataFormat(&c_dfDIMouse);
	// 排他制御レベルのセット
	mouse_->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
}

Input::~Input() {
	// 解放
	mouse_->Release();
	keyboard_->Release();
	directInput_->Release();
}

void Input::Update() {
	// 前フレームのキー入力状態
	memcpy(preKey_, key_, sizeof(key_));
	// キーボード情報の取得開始
	keyboard_->Acquire();
	// 全キーの入力状態を取得
	keyboard_->GetDeviceState(sizeof(key_), key_);

	// 前フレームのマウス入力状態
	preMouseState_ = mouseState_;
	// マウス情報の取得開始
	mouse_->Acquire();
	// クリック状態
	mouse_->GetDeviceState(sizeof(mouseState_), &mouseState_);
}
