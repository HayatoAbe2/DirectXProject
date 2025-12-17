#pragma once
#include "Window.h"
#include "../Io/DumpExporter.h"
#include "../Io/Logger.h"
#include "../Io/Audio.h"
#include "../Io/Input.h"
#include "../Graphics/directXContext.h"
#include "../Graphics/Renderer.h"
#include "../Object/ResourceManager.h"
#include "../Object/LightManager.h"
#include "../Scene/SceneManager.h"
#include "../Scene/GameContext.h"

#include <memory>
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

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "externals/DirectXTex/d3dx12.h"

#pragma comment(lib,"d3d12.lib")	
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")
#pragma comment(lib,"Dbghelp.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfuuid.lib")

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
	std::unique_ptr<D3DResourceLeakChecker> leakCheck_ = nullptr;

	// ダンプファイル作成
	std::unique_ptr<DumpExporter> dumpExporter_ = nullptr;

	// ログの出力
	std::unique_ptr<Logger> logger_ = nullptr;

	// ウィンドウの作成
	std::unique_ptr<Window> window_ = nullptr;

	// 音声の出力
	std::unique_ptr<Audio> audio_ = nullptr;

	// キー・コントローラー入力
	std::unique_ptr<Input> input_ = nullptr;

	// DirectX管理
	std::unique_ptr<DirectXContext> dxContext_ = nullptr;

	// 描画
	std::unique_ptr<Renderer> renderer_ = nullptr;

	// リソース管理
	std::unique_ptr<ResourceManager> resourceManager_ = nullptr;

	// ライト管理
	std::unique_ptr<LightManager> lightManager_ = nullptr;

	// ゲームシーンから利用できるデータや関数
	std::unique_ptr<GameContext> gameContext_ = nullptr;

	// シーン管理
	std::unique_ptr<SceneManager> sceneManager_ = nullptr;
};


