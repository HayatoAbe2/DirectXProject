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
#include "ModelData.h"
#include "VertexData.h"

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

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

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


ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);
MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

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
	window.Initialize(kClientWidth,kClientHeight);
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

	//-------------------------------------------------
	// XAudio2変数宣言
	//-------------------------------------------------
	Audio audio = Audio();
	audio.Initialize();
	logger.Log(logger.GetStream(), std::format("XAudio Initialized.\n"));

	//-------------------------------------------------
	// DirectInputの初期化
	//-------------------------------------------------
	Input *input = new Input(window.GetInstance(), window.GetHwnd());
	assert(&input);
	logger.Log(logger.GetStream(), std::format("DirectInput Initialized.\n"));

	// 描画のための設定
	Graphics graphics = Graphics();
	graphics.Initialize(kClientWidth,kClientHeight,window.GetHwnd());

	
	// UVTransform
	Transform uvTransformSprite{
		{1.0f,1.0f,0.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	// モデル
	const UINT modelCount = 1;
	struct Model {
		Transform transform;
	};
	Model model[modelCount] = {};
	// トランスフォームの初期化
	for (UINT i = 0; i < modelCount; ++i) {
		model[i].transform = Transform{
			{1.0f,1.0f,1.0f},
			{0.0f,0.0f,0.0f},
			{0.0f,0.0f,0.0f}
		};
	}

	// モデル読み込み
	ModelData modelData = LoadObjFile("Resources", "axis.obj");

	// テクスチャを読み込みSRV生成
	graphics.CreateSRV("Resources/uvChecker.png");
	graphics.CreateSRV(modelData.material.textureFilePath);

	// Transform
	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };
	Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	
	wchar_t sound1[] = L"Resources/Alarm01.wav";
	audio.SoundLoad(sound1);
	DebugCamera *debugCamera = new DebugCamera;
	debugCamera->Initialize();
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
			// デバッグカメラの更新
			debugCamera->Update(input);


			//-------------------------------------------------
			// ゲームの処理
			//-------------------------------------------------

			for (UINT i = 0; i < modelCount; ++i) {
				model[i].transform.rotate.y += 0.03f;
			}

			
			if (input->isTriggerRight()
				&& !debugCamera->IsEnable()) { // 右クリックの瞬間
				// サウンドの再生
				audio.SoundPlay(sound1);
			}

			// (更新処理終了後) 
			

			graphics.prepareDraw();
			graphics.BeginFrame();
			graphics.UpdateCamera(cameraTransform,*debugCamera);
			graphics.UpdateModel(0,model[0].transform);

			Material mat;
			mat.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			mat.enableLighting = true;
			mat.useTexture = true;
			mat.uvTransform = MakeIdentity4x4();
			graphics.UpdateMaterial(mat);

			graphics.UpdateSprite(transformSprite, uvTransformSprite, cameraTransform);
			graphics.DrawModel(modelData);
			
			graphics.EndFrame();
			
		}
	}
	
	audio.SoundUnload(sound1);
	audio.Finalize();
	MFShutdown();

	delete input;
	delete debugCamera;
	graphics.Finalize();
	
	CloseWindow(window.GetHwnd());

	CoUninitialize();


	return 0;
}

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	// 変数の宣言
	ModelData modelData; // 構築するModeldata
	std::vector<Vector4> positions; // 位置
	std::vector<Vector3> normals; // 法線
	std::vector<Vector2> texcoords; // テクスチャ座標
	std::string line; // ファイルから読んだ1行を格納するもの

	// ファイルを開く
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open()); // 開けなかったらエラー

	// ファイルを読み、ModelDataを構築
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; // 先頭の識別子を読む
		// 識別子に応じた処理
		if (identifier == "v") { // 頂点座標
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f; // 右手座標系から左手座標系への変換
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") { // テクスチャ座標
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y; // 左下原点から左上原点への変換
			texcoords.push_back(texcoord);
		} else if (identifier == "vn") { // 法線
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f; // 右手座標系から左手座標系への変換
			normals.push_back(normal);
		} else if (identifier == "f") {
			VertexData triangle[3];
			// 面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				// 頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/'); // /区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				// 要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1]; // 1始まりなので-1
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				triangle[faceVertex] = { position, texcoord, normal };
			}
			// 頂点を逆順で登録することで、周り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		} else if (identifier == "mtllib") {
			// materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			// 基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}

	return modelData;
}

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	// 変数の宣言
	MaterialData materialData; // 構築するMaterialData
	std::string line; // ファイルから読んだ1行を格納するもの
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open()); // 開けなかったらエラー

	// ファイルを読み、MaterialDataを構築
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		// identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			// 連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	return materialData;
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