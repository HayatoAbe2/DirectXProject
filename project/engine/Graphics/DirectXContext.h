#pragma once
#pragma once
#include "../Io/Logger.h"
#include "../Scene/DebugCamera.h"
#include "../Object/Material.h"
#include "../Object/Model.h"
#include "../Object/Transform.h"
#include "../Object/TransformationMatrix.h"
#include "../Object/LightManager.h"
#include "../Object/VertexData.h"
#include "../Object/Sprite.h"
#include "../Graphics/DeviceManager.h"
#include "../Graphics/CommandListManager.h"
#include "../Graphics/RootSignatureManager.h"
#include "../Graphics/ShaderCompiler.h"
#include "../Graphics/RenderTargetManager.h"
#include "../Graphics/DescriptorHeapManager.h"
#include "../Graphics/SRVManager.h"
#include "../Graphics/PipelineStateManager.h"
#include "../Graphics/FixFPS.h"
#include "../Object/ResourceManager.h"
#include "../Graphics/ImGuiManager.h"
#include "../Graphics/BlendMode.h"

#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <dxcapi.h>

#include "externals/DirectXTex/DirectXTex.h"

class DirectXContext {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(int32_t clientWidth, int32_t clientHeight, HWND hwnd, Logger* logger);

	/// <summary>
	/// 解放処理(ループ終了後に行う)
	/// </summary>
	void Finalize();

	/// <summary>
	/// フレーム開始時の処理(描画開始時に行う)
	/// </summary>
	void BeginFrame();

	/// <summary>
	/// フレーム終了時の処理(描画終了時に行う)
	/// </summary>
	void EndFrame();

	/// <summary>
	/// バッファリソース作成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="sizeInBytes">バッファサイズ</param>
	/// <returns>作成したリソース</returns>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	// アクセサ
	int32_t GetWindowWidth() { return clientWidth_; }
	int32_t GetWindowHeight() { return clientHeight_; }
	DeviceManager* GetDeviceManager() { return deviceManager_.get(); }
	CommandListManager* GetCommandListManager() { return commandListManager_.get(); }
	DescriptorHeapManager* GetDescriptorHeapManager() { return descriptorHeapManager_.get(); }
	SRVManager* GetSRVManager() { return srvManager_.get(); }
	PipelineStateManager* GetPipelineStateManager() { return pipelineStateManager_.get(); }
	RootSignatureManager* GetRootSignatureManager() { return rootSignatureManager_.get(); }

private:


	/// <summary>
	/// SwapChain初期化
	/// </summary>
	void InitializeSwapChain(HWND hwnd);

	/// <summary>
	/// Viewport,Scissor設定
	/// </summary>
	void SetViewportAndScissor();

	/// <summary>
	/// TextureResource作成
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(
		const Microsoft::WRL::ComPtr<ID3D12Device>& device, int32_t width, int32_t height);

	// 画面サイズ
	int32_t clientWidth_;
	int32_t clientHeight_;

	// DXGIファクトリー
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;

	// デバイスマネージャー
	std::unique_ptr<DeviceManager> deviceManager_ = nullptr;

	// スワップチェーン
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};

	UINT backBufferIndex_;

	// 深度ステンシルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;

	// 遷移バリア
	D3D12_RESOURCE_BARRIER barrier_ = {};

	// ビューポート
	D3D12_VIEWPORT viewport_ = {};

	// シザー矩形
	D3D12_RECT scissorRect_ = {};

	// ログクラス
	Logger* logger_;

	// シェーダーコンパイルクラス
	std::unique_ptr<ShaderCompiler> shaderCompiler_ = nullptr;

	// ディスクリプタヒープ管理クラス
	std::unique_ptr<DescriptorHeapManager> descriptorHeapManager_ = nullptr;

	// RTV設定など
	std::unique_ptr<RenderTargetManager> renderTargetManager_ = nullptr;

	// SRVマネージャー
	std::unique_ptr<SRVManager> srvManager_ = nullptr;

	// リソース管理クラス
	std::unique_ptr<ResourceManager> resourceManager_ = nullptr;

	// コマンド関連
	std::unique_ptr<CommandListManager> commandListManager_ = nullptr;

	// ルートシグネチャ管理クラス
	std::unique_ptr<RootSignatureManager> rootSignatureManager_ = nullptr;

	// パイプラインステート管理クラス
	std::unique_ptr<PipelineStateManager> pipelineStateManager_ = nullptr;

	// FPS固定クラス
	std::unique_ptr<FixFPS> fixFPS_ = nullptr;

	// ImGui管理クラス
	std::unique_ptr<ImGuiManager > imGuiManager_ = nullptr;
};

