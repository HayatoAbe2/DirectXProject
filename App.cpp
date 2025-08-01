#include "App.h"
#include "DumpExporter.h"
#include "Logger.h"
#include "Window.h"
#include "Audio.h"
#include "Input.h"
#include "Graphics.h"
#include "SceneManager.h"
#include <format>
#include <cassert>

void App::Initialize() {
	// リソースリーク確認(最初に作る)
	leakCheck_ = new D3DResourceLeakChecker; // 最後に解放する

	// COMの初期化
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));

	// MFの初期化
	MFStartup(MF_VERSION);

	// ダンプ作成機能
	dumpExporter_ = new DumpExporter();
	// クラッシュ時ダンプファイルに記録する
	SetUnhandledExceptionFilter(dumpExporter_->ExportDump);

	// ロガー
	logger_ = new Logger;

	// ウィンドウの生成
	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;
	window_ = new Window;
	window_->Initialize(kClientWidth, kClientHeight);
	logger_->Log(logger_->GetStream(), std::format("Window Initialized.\n"));

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
	audio_ = new Audio();
	audio_->Initialize();
	assert(&audio_);
	logger_->Log(logger_->GetStream(), std::format("XAudio Initialized.\n"));

	// DirectInputの初期化
	input_ = new Input(window_->GetInstance(), window_->GetHwnd());
	assert(&input_);
	logger_->Log(logger_->GetStream(), std::format("DirectInput Initialized.\n"));

	// 描画クラス
	graphics_ = new Graphics();
	graphics_->Initialize(kClientWidth,kClientHeight,window_->GetHwnd(),logger_);
	logger_->Log(logger_->GetStream(), std::format("Graphics Initialized.\n"));

	// シーンマネージャー
	sceneManager_ = new SceneManager();
	sceneManager_->Initialize(graphics_);
}

void App::Run() {

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
			input_->Update();

			//-------------------------------------------------
			// ゲームの処理
			//-------------------------------------------------

			// 更新処理
			sceneManager_->Update(input_);

			// 描画開始時に呼ぶ
			graphics_->BeginFrame();

			// 描画処理
			sceneManager_->Draw(graphics_);
			
			// 描画終了時に呼ぶ
			graphics_->EndFrame();
		}
	}
}

void App::Finalize() {
	
	// シーンマネージャー
	sceneManager_->Finalize();
	delete sceneManager_;

	// XAudio終了処理
	audio_->Finalize();
	delete audio_;
	MFShutdown();
	logger_->Log(logger_->GetStream(), std::format("XAudio Finalized.\n"));

	// 入力
	delete input_;
	logger_->Log(logger_->GetStream(), std::format("DirectInput Finalized.\n"));

	// 描画クラス実体解放
	graphics_->Finalize();
	delete graphics_;
	logger_->Log(logger_->GetStream(), std::format("Graphics Finalized.\n"));

	// ウィンドウ終了
	CloseWindow(window_->GetHwnd());
	delete window_;
	logger_->Log(logger_->GetStream(), std::format("Window Closed.\n"));

	CoUninitialize();

	// ログ
	delete logger_;

	// ダンプ作成クラス
	delete dumpExporter_;

	// 解放されていないリソースがあれば警告、停止
	delete leakCheck_;
}