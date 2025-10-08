#pragma once
#include <Windows.h>
#include "../../externals/imgui/imgui.h"
#include "../../externals/imgui/imgui_impl_win32.h"
#include <cstdint>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class Window {
public:
	void Initialize(int32_t clientWidth, int32_t clientHeight);
	HWND GetHwnd() const { return hwnd_; }
	HINSTANCE GetInstance() const { return wc_.hInstance; }

private:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	HWND hwnd_ = nullptr;
	WNDCLASS wc_ = {};

	// ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0,0,1280,720 };
};
