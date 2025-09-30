#pragma once
#include <Windows.h>
#include <cstdint>
#include <format>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dbghelp.h>
#include <strsafe.h>
#include <vector>
#include <wrl.h>
#include <mfapi.h>

#include "../../externals/imgui/imgui.h"
#include "../../externals/imgui/imgui_impl_dx12.h"
#include "../../externals/imgui/imgui_impl_win32.h"
#include "../../externals/DirectXTex/d3dx12.h"

#pragma comment(lib,"d3d12.lib")	
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")
#pragma comment(lib,"Dbghelp.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfuuid.lib")

class DumpExporter;
class Logger;
class Window;
class Audio;
class Input;
class Graphics;
class SceneManager;

/// <summary>
/// アプリケーション全体の処理
/// </summary>
class App {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 処理
	/// </summary>
	void Run();

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

private:
	
	// ダンプファイル作成
	DumpExporter* dumpExporter_ = nullptr;

	// ログの出力
	Logger* logger_ = nullptr;

	// ウィンドウの作成
	Window* window_ = nullptr;

	// 音声の出力
	Audio* audio_ = nullptr;

	// キー・コントローラー入力
	Input* input_ = nullptr;

	// 描画
	Graphics* graphics_ = nullptr;

	// シーン管理
	SceneManager* sceneManager_ = nullptr;

	struct D3DResourceLeakChecker {
		~D3DResourceLeakChecker() {
			// リソースリークチェック
			Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
			if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
				debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
				debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
				debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			}
		}
	};
	D3DResourceLeakChecker* leakCheck_ = nullptr;
};


