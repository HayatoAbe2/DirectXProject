#include "Input.h"

Input::Input(HINSTANCE hInstance, HWND hwnd) {
	HRESULT hr;

	// オブジェクト生成
	hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput_, nullptr);
	assert(SUCCEEDED(hr));

	///
	/// キーボード
	///

	// デバイスの生成
	hr = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(hr));
	// 入力データ形式のセット
	hr = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));
	// 排他制御レベルのセット
	hr = keyboard_->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));

	///
	/// マウス
	///

	// デバイス生成
	hr = directInput_->CreateDevice(GUID_SysMouse, &mouse_, NULL);
	assert(SUCCEEDED(hr));
	// 入力データ形式のセット
	hr = mouse_->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(hr));
	// 排他制御レベルのセット
	hr = mouse_->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));

	///
	/// コントローラー
	///

	// コールバック関数
	auto EnumJoysticksCallback = [](const DIDEVICEINSTANCE* pdidInstance, VOID* pContext) -> BOOL {
		auto pDI = reinterpret_cast<LPDIRECTINPUT8>(pContext);
		LPDIRECTINPUTDEVICE8* ppController = reinterpret_cast<LPDIRECTINPUTDEVICE8*>(pContext);
		LPDIRECTINPUTDEVICE8 tmpDevice = nullptr;

		HRESULT hr = pDI->CreateDevice(pdidInstance->guidInstance, &tmpDevice, NULL);
		if (SUCCEEDED(hr)) {
			*ppController = tmpDevice;
			return DIENUM_STOP; // 最初の1つだけ使用
		}
		return DIENUM_CONTINUE;
		};

	// コントローラーデバイス列挙
	directInput_->EnumDevices(
			DI8DEVCLASS_GAMECTRL,
			[](const DIDEVICEINSTANCE* pdidInstance, VOID* pContext) -> BOOL {
				auto self = reinterpret_cast<Input*>(pContext);
				if (SUCCEEDED(self->directInput_->CreateDevice(pdidInstance->guidInstance, &self->controller_, NULL))) {
					self->controller_->SetDataFormat(&c_dfDIJoystick2);
					self->controller_->SetCooperativeLevel(self->hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
					self->SetRange();
					return DIENUM_STOP;
				}
				return DIENUM_CONTINUE;
			},
			this,
			DIEDFL_ATTACHEDONLY
		);

	// コントローラーが見つかった場合 
	if (controller_) {
		controller_->Acquire();
	}

	hwnd_ = hwnd;
}

Input::~Input() {
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

	// 前フレームのコントローラー入力状態
	preControllerState_ = controllerState_;
	if (controller_) {
		HRESULT hr = controller_->Poll();
		if (FAILED(hr)) {
			hr = controller_->Acquire();
			if (FAILED(hr)) {
				// 再試行
				controller_->Release();
				controller_ = nullptr;
			}
		}

		if (controller_) {
			controller_->GetDeviceState(sizeof(controllerState_), &controllerState_);
		}

	} else {
		// コントローラーがない場合
		directInput_->EnumDevices(
			DI8DEVCLASS_GAMECTRL,
			[](const DIDEVICEINSTANCE* pdidInstance, VOID* pContext) -> BOOL {
				auto self = reinterpret_cast<Input*>(pContext);
				if (SUCCEEDED(self->directInput_->CreateDevice(pdidInstance->guidInstance, &self->controller_, NULL))) {
					self->controller_->SetDataFormat(&c_dfDIJoystick2);
					self->controller_->SetCooperativeLevel(self->hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
					self->SetRange();
					return DIENUM_STOP;
				}
				return DIENUM_CONTINUE;
			},
			this,
			DIEDFL_ATTACHEDONLY
		);
	}
}

void Input::SetRange() {
	// 軸範囲を設定（-1000～+1000）
	DIPROPRANGE diprg;
	diprg.diph.dwSize = sizeof(DIPROPRANGE);
	diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	diprg.diph.dwHow = DIPH_BYOFFSET;
	diprg.lMin = -1000;
	diprg.lMax = +1000;

	diprg.diph.dwObj = DIJOFS_X;
	controller_->SetProperty(DIPROP_RANGE, &diprg.diph);
	diprg.diph.dwObj = DIJOFS_Y;
	controller_->SetProperty(DIPROP_RANGE, &diprg.diph);
}