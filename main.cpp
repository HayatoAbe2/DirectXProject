#include <Windows.h>
#include <cstdint>
#include <string>
#include <format>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <dbghelp.h>
#include <strsafe.h>
#include <vector>
#include <sstream>
#include <wrl.h>
#include <fstream>
#include <mfapi.h>

#include "Input.h"
#include "Audio.h"
#include "Math.h"
#include "Logger.h"
#include "DebugCamera.h"
#include "Window.h"
#include "Graphics.h"
#include "Material.h"
#include "Model.h"
#include "VertexData.h"

#include "TitleScene.h"
#include "GameScene.h"

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"

#pragma comment(lib,"d3d12.lib")	
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")
#pragma comment(lib,"Dbghelp.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfuuid.lib")

// ログ
Logger logger;

static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception);

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

// チャンクヘッダ
struct ChunkHeader {
	char id[4]; // チャンク用のID
	int32_t size; // チャンクサイズ
};

// RIFFヘッダチャンク
struct RiffHeader {
	ChunkHeader chunk; // "RIFF"
	char type[4]; // "WAVE"
};

// FMTチャンク
struct FormatChunk {
	ChunkHeader chunk; // "fmt "
	WAVEFORMATEX fmt; // 波形フォーマット
};

// シーン
TitleScene* titleScene = nullptr;
GameScene* gameScene = nullptr;

enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,
};

Scene scene = Scene::kUnknown; // 初期状態
void ChangeScene(Input* input,HWND hwnd);
void UpdateScene();
void DrawScene();

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	// リソースリーク確認(最初に作る)
	D3DResourceLeakChecker leakCheck; // 最後に解放する

	// COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	// MFの初期化
	MFStartup(MF_VERSION);

	SetUnhandledExceptionFilter(ExportDump);

	// ウィンドウの生成
	Window window;
	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;
	window.Initialize(kClientWidth, kClientHeight);
	logger.Log(logger.GetStream(), logger.ConvertString(std::format(L"Window Initialized.\n")));

#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		// デバッグレイヤーを有効にする
		debugController->EnableDebugLayer();
		// さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

	// XAudio2
	Audio audio = Audio();
	audio.Initialize();
	logger.Log(logger.GetStream(), std::format("XAudio Initialized.\n"));

	// DirectInputの初期化
	Input* input = new Input(window.GetInstance(), window.GetHwnd());
	assert(&input);
	logger.Log(logger.GetStream(), std::format("DirectInput Initialized.\n"));

	// 描画のための設定
	Graphics graphics = Graphics(); // 初期化と終了は各シーンで行う
	// シーン初期化
	scene = Scene::kTitle;
	titleScene = new TitleScene;
	titleScene->Initialize(input, window.GetHwnd());

	//-------------------------------------------------
	// メインループ
	//-------------------------------------------------

	MSG msg{};

	// ウィンドウの×ボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		// Windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {

			// キーボード・マウス入力の更新
			input->Update();

			//-------------------------------------------------
			// ゲームの処理
			//-------------------------------------------------

			ChangeScene(input, window.GetHwnd());
			UpdateScene();

			// 更新処理終わり

			DrawScene();

		}
	}

	audio.Finalize();
	MFShutdown();

	delete input;
	delete titleScene;
	delete gameScene;

	CloseWindow(window.GetHwnd());

	CoUninitialize();


	return 0;
}

static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception) {
	// 時刻を取得して、時刻を名前に入れたファイルを作成。Dumpディレクトリ以下に出力
	SYSTEMTIME time;
	GetLocalTime(&time);
	wchar_t filePath[MAX_PATH] = { 0 };
	CreateDirectory(L"./Dumps", nullptr);
	StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
	HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	// processId(このexeのId)とクラッシュ(例外)の発生したthreadIdを取得
	DWORD processId = GetCurrentProcessId();
	DWORD threadId = GetCurrentThreadId();
	// 設定情報を入力
	MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{ 0 };
	minidumpInformation.ThreadId = threadId;
	minidumpInformation.ExceptionPointers = exception;
	minidumpInformation.ClientPointers = TRUE;
	// Dumpを出力。MiniDumpNormalは最低限の情報を出力するフラグ
	MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &minidumpInformation, nullptr, nullptr);
	// 他に関連付けられているSEH例外ハンドラがあれば実行。通常はプロセスを終了する
	return EXCEPTION_EXECUTE_HANDLER;
}

void ChangeScene(Input *input,HWND hwnd) {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene->isFinished()) {
			// シーン変更
			scene = Scene::kGame;
			// 旧シーン
			delete titleScene;
			titleScene = nullptr;
			// 新シーン
			gameScene = new GameScene;
			gameScene->Initialize(input, hwnd);
		}
		break;

	case Scene::kGame:
		if (gameScene->isFinished()) {
			// シーン変更
			scene = Scene::kTitle;
			// 旧シーン
			delete gameScene;
			gameScene = nullptr;
			// 新シーン
			titleScene = new TitleScene;
			titleScene->Initialize(input, hwnd);
		}
		break;
	}
}

void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	}
}

void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	}
}
