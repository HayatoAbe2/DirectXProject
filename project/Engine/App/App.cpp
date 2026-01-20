#include "App.h"
#include "../App/Window.h"
#include "../Graphics/DeviceManager.h"
#include "../Io/DumpExporter.h"
#include "../Io/Logger.h"
#include "../Io/Audio.h"
#include "../Io/Input.h"
#include "../Object/ResourceManager.h"
#include "../Object/LightManager.h"
#include "../Scene/SceneManager.h"
#include "../Scene/GameContext.h"

#include <format>
#include <cassert>

void App::Initialize() {
	// リソースリーク確認(最初に作る)
	leakCheck_ = std::make_unique<D3DResourceLeakChecker>(); // 最後に解放する

	// COMの初期化
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));

	// ダンプ作成機能
	dumpExporter_ = std::make_unique<DumpExporter>();
	// クラッシュ時ダンプファイルに記録する
	SetUnhandledExceptionFilter(dumpExporter_->ExportDump);

	// ロガー
	logger_ = std::make_unique<Logger>();

	// ウィンドウの生成
	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;
	window_ = std::make_unique<Window>();
	window_->Initialize(kClientWidth, kClientHeight);
	logger_->Log(logger_->GetStream(), std::format("[Window] Initialization complete.\n"));

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
	audio_ = std::make_unique<Audio>();
	audio_->Initialize();
	assert(&audio_);
	logger_->Log(logger_->GetStream(), std::format("[Audio] Initialization complete.\n"));

	// DirectInputの初期化
	input_ = std::make_unique<Input>(window_->GetInstance(), window_->GetHwnd());
	assert(&input_);
	logger_->Log(logger_->GetStream(), std::format("[Input] Initialization complete.\n"));

	// DirectX準備
	dxContext_ = std::make_unique<DirectXContext>();
	dxContext_->Initialize(kClientWidth, kClientHeight, window_->GetHwnd(), logger_.get());

	// 描画クラス
	renderer_ = std::make_unique<Renderer>();
	renderer_->Initialize(dxContext_.get());
	logger_->Log(logger_->GetStream(), std::format("[Renderer] Initialization complete.\n"));

	// リソース
	resourceManager_ = std::make_unique<ResourceManager>();
	resourceManager_->Initialize(dxContext_->GetDeviceManager()->GetDevice(), dxContext_->GetCommandListManager(), dxContext_->GetDescriptorHeapManager(), dxContext_->GetSRVManager(), logger_.get());
	logger_->Log(logger_->GetStream(), std::format("[ResourceManager] Initialization complete.\n"));

	// ライト
	lightManager_ = std::make_unique<LightManager>();
	lightManager_->Initialize(resourceManager_.get());

	// コンテキスト
	gameContext_ = std::make_unique<GameContext>(renderer_.get(), audio_.get(), input_.get(), resourceManager_.get(), lightManager_.get());

	// シーンマネージャー
	sceneManager_ = std::make_unique<SceneManager>(gameContext_.get());
	sceneManager_->Initialize();
	logger_->Log(logger_->GetStream(), std::format("[SceneManager] Initialization complete.\n"));

	// 音声データの読み込み
	audio_->SoundLoad(L"Resources/Sounds/BGM/field.mp3");
	audio_->SoundPlay(L"Resources/Sounds/BGM/field.mp3", true,0.5f);
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
			sceneManager_->Update();

			// 描画開始時に呼ぶ
			renderer_->BeginFrame();

			// 描画処理
			sceneManager_->Draw();

			// 描画終了時に呼ぶ
			renderer_->EndFrame();
		}
	}
}

void App::Finalize() {
	{
		// シーンマネージャー
		sceneManager_->Finalize();
		sceneManager_.reset();

		// 入力
		input_.reset();
		logger_->Log(logger_->GetStream(), std::format("[Input] Shutdown complete.\n"));

		lightManager_.reset();
		logger_->Log(logger_->GetStream(), std::format("[LightManager] Shutdown complete.\n"));

		resourceManager_.reset();
		logger_->Log(logger_->GetStream(), std::format("[ResourceManager] Shutdown complete.\n"));

		gameContext_.reset();

		// Audio
		audio_->StopAll();
		audio_->SoundUnload(L"Resources/Sounds/SE/press.mp3");
		audio_->SoundUnload(L"Resources/Sounds/SE/explosion.mp3");
		audio_->SoundUnload(L"Resources/Sounds/SE/shoot.mp3");
		audio_->SoundUnload(L"Resources/Sounds/SE/fire.mp3");
		audio_->SoundUnload(L"Resources/Sounds/SE/floorClear.mp3");
		audio_->SoundUnload(L"Resources/Sounds/SE/fall.mp3");
		audio_->SoundUnload(L"Resources/Sounds/SE/warp.mp3");
		audio_->SoundUnload(L"Resources/Sounds/SE/hit.mp3");
		audio_->Finalize();
		audio_.reset();
		logger_->Log(logger_->GetStream(), std::format("[Audio] Shutdown complete.\n"));

		// 描画クラス実体解放
		renderer_.reset();
		logger_->Log(logger_->GetStream(), std::format("[Renderer] Shutdown complete.\n"));

		// dx
		dxContext_->Finalize();
		dxContext_.reset();


		// ウィンドウ終了
		CloseWindow(window_->GetHwnd());
		window_.reset();
		logger_->Log(logger_->GetStream(), std::format("[Window] Closed.\n"));

		CoUninitialize();

		// ログ
		logger_.reset();

		// ダンプ作成クラス
		dumpExporter_.reset();

	}
	// 解放されていないリソースがあれば警告、停止
	leakCheck_.reset();
}