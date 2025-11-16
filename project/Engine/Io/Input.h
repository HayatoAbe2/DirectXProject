#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <cstdint>
#include <wrl.h>
#include "../Math/MathUtils.h"
class Input {
public:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	Input(HINSTANCE hInstance, HWND hwnd);
	~Input();
	void Update();

	void SetRange();

	//
	// キー入力関連
	//

	/// <summary>
	/// キーを押した瞬間か
	/// </summary>
	/// <param name="keyNumber">キー番号</param>
	/// <returns>キーを押した瞬間のみtrue</returns>
	bool IsTrigger(uint8_t keyNumber) { return (!preKey_[keyNumber] && key_[keyNumber]); };

	/// <summary>
	/// キーを押している状態か
	/// </summary>
	/// <param name="keyNumber">キー番号</param>
	/// <returns>キーが押されていればtrue</returns>
	bool IsPress(uint8_t keyNumber) { return (key_[keyNumber]); };

	/// <summary>
	/// キーを離した瞬間か
	/// </summary>
	/// <param name="keyNumber">キー番号</param>
	/// <returns>キーが離された瞬間のみtrue</returns>
	bool IsRelease(uint8_t keyNumber) { return (preKey_[keyNumber] && !key_[keyNumber]); };

	//
	// マウス入力関連
	//

	bool IsClickLeft() { return (mouseState_.rgbButtons[0] & 0x80); }
	bool IsClickRight() { return (mouseState_.rgbButtons[1] & 0x80); }
	bool IsClickWheel() { return (mouseState_.rgbButtons[2] & 0x80); }
	bool IsTriggerLeft() {
		return !(preMouseState_.rgbButtons[0] & 0x80) &&
			(mouseState_.rgbButtons[0] & 0x80);
	}
	bool IsTriggerRight() {
		return !(preMouseState_.rgbButtons[1] & 0x80) &&
			(mouseState_.rgbButtons[1] & 0x80);
	}
	bool IsTriggerWheel() {
		return !(preMouseState_.rgbButtons[2] & 0x80) &&
			(mouseState_.rgbButtons[2] & 0x80);
	}

	Vector3 GetMouseMove() { return { float(mouseState_.lX),float(mouseState_.lY),float(mouseState_.lZ) }; };
	Vector2 GetMousePosition() {
		POINT mousePosition;
		// マウスの位置を取得
		GetCursorPos(&mousePosition);
		// ウィンドウのクライアント座標に変換
		ScreenToClient(hwnd_, &mousePosition);
		return { float(mousePosition.x), float(mousePosition.y) };
	}

	//
	// コントローラー入力関連
	//

	bool IsControllerPress(uint8_t buttonNumber) {
		return (controllerState_.rgbButtons[buttonNumber] & 0x80);
	}

	Vector2 GetLeftStick() {
		// -1000~1000の範囲を-1.0f~1.0fに正規化
		Vector2 stick = {
		controllerState_.lX / 1000.0f,
		controllerState_.lY / 1000.0f
		};

		if (fabs(stick.x) < deadZone_) stick.x = 0.0f;
		if (fabs(stick.y) < deadZone_) stick.y = 0.0f;

		return stick;
	}

	Vector2 GetRightStick() {
		Vector2 stick = {
			controllerState_.lRx / 1000.0f,
			controllerState_.lRy / 1000.0f
		};

		if (fabs(stick.x) < deadZone_) stick.x = 0.0f;
		if (fabs(stick.y) < deadZone_) stick.y = 0.0f;

		return stick;
	}

	HWND GetHwnd() { return hwnd_; }

private:
	ComPtr<IDirectInput8> directInput_ = nullptr;
	ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
	ComPtr<IDirectInputDevice8> mouse_ = nullptr;
	ComPtr<IDirectInputDevice8> controller_ = nullptr;

	// キー入力状態
	BYTE preKey_[256]{};
	BYTE key_[256]{};

	// マウス入力状態
	DIMOUSESTATE preMouseState_{};
	DIMOUSESTATE mouseState_{};

	// コントローラー入力状態
	DIJOYSTATE2 preControllerState_{};
	DIJOYSTATE2 controllerState_{};

	// スティックのデッドゾーン
	const float deadZone_ = 0.1f;

	HWND hwnd_{}; // ウィンドウハンドル

};

