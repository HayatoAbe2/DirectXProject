#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <cstdint>
#include "Math.h"
class Input {
public:
	Input(HINSTANCE hInstance, HWND hwnd);
	~Input();
	void Update();

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

	bool isClickLeft() { return (mouseState_.rgbButtons[0] & 0x80); }
	bool isClickRight() { return (mouseState_.rgbButtons[1] & 0x80); }
	bool isClickWheel() { return (mouseState_.rgbButtons[2] & 0x80); }
	bool isTriggerLeft() {
		return !(preMouseState_.rgbButtons[0] & 0x80) &&
			(mouseState_.rgbButtons[0] & 0x80);
	}
	bool isTriggerRight() {
		return !(preMouseState_.rgbButtons[1] & 0x80) &&
			(mouseState_.rgbButtons[1] & 0x80);
	}
	bool isTriggerWheel() {
		return !(preMouseState_.rgbButtons[2] & 0x80) &&
			(mouseState_.rgbButtons[2] & 0x80);
	}

	Vector3 GetMouseMove() { return { float(mouseState_.lX),float(mouseState_.lY),float(mouseState_.lZ) }; };
	
private:
	IDirectInput8* directInput_;
	IDirectInputDevice8* keyboard_;
	IDirectInputDevice8* mouse_;

	BYTE preKey_[256];
	BYTE key_[256];
	DIMOUSESTATE preMouseState_;
	DIMOUSESTATE mouseState_;

};

