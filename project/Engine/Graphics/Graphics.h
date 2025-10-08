#pragma once
#include "../Io/Logger.h"
#include "Math.h"
#include "../Scene/DebugCamera.h"
#include "../Object/Material.h"
#include "../Object/Model.h"
#include "../Object/Transform.h"
#include "../Object/TransformationMatrix.h"
#include "../Scene/DirectionalLight.h"
#include "../Object/VertexData.h"

#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <dxcapi.h>

#include "externals/DirectXTex/DirectXTex.h"

class Sprite;
class DeviceManager;
class CommandListManager;
class RootSignatureManager;
class ShaderCompiler;
class RenderTargetManager;
class DescriptorHeapManager;
class PipelineStateManager;

class ResourceManager;

class Graphics {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(int32_t clientWidth, int32_t clientHeight, HWND hwnd, Logger* logger);

	/// <summary>
	/// モデル描画
	/// </summary>
	/// <param name="model">描画するモデル</param>
	void DrawModel(Model& model, bool useAlphaBlend = 0);

	/// <summary>
	/// スプライト描画
	/// </summary>
	/// <param name="sprite">描画するスプライト</param>
	void DrawSprite(Sprite& sprite);

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
	int32_t GetWindowWidth() { return clientWidth_; };
	int32_t GetWindowHeight() { return clientHeight_; };
	DeviceManager* GetDeviceManager() { return deviceManager_; };
	CommandListManager* GetCommandListManager() { return commandListManager_; };
	DescriptorHeapManager* GetDescriptorHeapManager() { return descriptorHeapManager_; };

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
	/// DirectionalLight初期化
	/// </summary>
	void CreateLightBuffer();

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
	DeviceManager* deviceManager_ = nullptr;

	// スワップチェーン
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
	
	UINT backBufferIndex_;


	// 深度ステンシルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;

	// 遷移バリア
	D3D12_RESOURCE_BARRIER barrier_ = {};
	
	TransformationMatrix* transformationMatrixData_ = nullptr;
	TransformationMatrix* transformationMatrixDataSprite_ = nullptr;

	D3D12_VIEWPORT viewport_ = {};
	D3D12_RECT scissorRect_ = {};

	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceSprite_ = nullptr;
	
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_ = nullptr;
	DirectionalLight* directionalLightData_;

	// ログクラス
	Logger* logger_;

	// シェーダーコンパイルクラス
	ShaderCompiler* shaderCompiler_ = nullptr;

	// ディスクリプタヒープ管理クラス
	DescriptorHeapManager* descriptorHeapManager_ = nullptr;

	// RTV設定など
	RenderTargetManager* renderTargetManager_ = nullptr;

	// リソース管理クラス
	ResourceManager* resourceManager_ = nullptr;

	// コマンド関連
	CommandListManager* commandListManager_ = nullptr;

	// ルートシグネチャ管理クラス
	RootSignatureManager* rootSignatureManager_ = nullptr;

	// パイプラインステート管理クラス
	PipelineStateManager* pipelineStateManager_ = nullptr;
};