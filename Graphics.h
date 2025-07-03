#pragma once
#include <wrl.h>
#include <dxgi1_6.h>
#include "Logger.h"
#include <d3d12.h>
#include "externals/DirectXTex/DirectXTex.h"
#include "Math.h"
#include "DebugCamera.h"
#include "Material.h"
#include <dxcapi.h>
#include "Model.h"
#include "DictionalLight.h"
#include "Transform.h"
#include "TransformationMatrix.h"
#include "VertexData.h"

class Sprite;
class Graphics {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(int32_t clientWidth, int32_t clientHeight, HWND hwnd);

	/// <summary>
	/// SRV作成
	/// </summary>
	Model* CreateSRV(Model* model);
	Sprite* CreateSRV(Sprite* sprite);

	
	// 描画処理
	void DrawModel(Model& model);
	void DrawSprite(Sprite& sprite);

	// 解放
	void Finalize();

	/// <summary>
	/// フレーム開始時の処理
	/// </summary>
	void BeginFrame();

	/// <summary>
	/// フレーム終了時の処理
	/// </summary>
	void EndFrame();

	/// <summary>
	/// バッファリソース作成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="sizeInBytes">バッファサイズ</param>
	/// <returns>作成したリソース</returns>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes);

	Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() const {
		return device_;
	}

	int32_t GetWindowWidth() { return clientWidth_; };
	int32_t GetWindowHeight() { return clientHeight_; };

private:

	void SelectAdapter();

	void CreateD3D12Device();
	void DebugFilter();
	void InitializeFence();
	void InitializeCommandQueue();
	void InitializeCommandAllocator();
	void InitializeCommandList();
	void InitializeSwapChain(HWND hwnd);
	void InitializeDescriptorHeaps();
	void CreateRootSignature();
	void CreatePipelineState();

	IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler, std::ostream& os);

	void SetViewportAndScissor();

	void InitializeImGui(HWND hwnd);

	void CreateLightBuffer();

	DirectX::ScratchImage LoadTexture(const std::string& filePath);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const DirectX::TexMetadata& metadata);

	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages, const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList);

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
		const Microsoft::WRL::ComPtr<ID3D12Device>& device,
		D3D12_DESCRIPTOR_HEAP_TYPE heapType,
		UINT numDescriptors,
		bool shaderVisible);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
		const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap,
		uint32_t descriptorSize, uint32_t index);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(
		const Microsoft::WRL::ComPtr<ID3D12Device>& device, int32_t width, int32_t height);


	int32_t clientWidth_;
	int32_t clientHeight_;
	// DXGIファクトリー
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;

	// 使用するアダプタ用の変数
	IDXGIAdapter4* useAdapter_ = nullptr;

	// D3D12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;

	// フェンス
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;
	HANDLE fenceEvent_ = nullptr;
	uint64_t fenceValue_ = 0;

	// コマンドキュー
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc_{};
	// コマンドアロケータ
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	// コマンドリスト
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;

	// スワップチェーン
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
	// スワップチェーンのリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2] = { nullptr, nullptr };
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	UINT backBufferIndex_;

	// RTV
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;

	// SRV
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_ = nullptr;
	UINT currentSRVIndex_ = 0; // SRVのインデックス

	// DSV
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_ = nullptr;

	// 各DescriptorHeapのサイズ
	uint32_t descriptorSizeSRV_;
	uint32_t descriptorSizeRTV_;
	uint32_t descriptorSizeDSV_;

	// 深度ステンシルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;

	// 遷移バリア
	D3D12_RESOURCE_BARRIER barrier_ = {};

	// inputLayout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_;

	// dxc
	IDxcUtils* dxcUtils_;
	IDxcCompiler3* dxcCompiler_;
	IDxcIncludeHandler* includeHandler_;

	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc_{};

	// ResterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc_ = {};

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};

	// シェーダー
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;

	// シグネチャ
	ID3DBlob* signatureBlob_ = nullptr;

	ID3DBlob* errorBlob_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

	// ログ
	Logger logger_;


	TransformationMatrix* transformationMatrixData_ = nullptr;
	TransformationMatrix* transformationMatrixDataSprite_ = nullptr;

	D3D12_VIEWPORT viewport_ = {};
	D3D12_RECT scissorRect_ = {};

	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceSprite_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_ = nullptr;
	DirectionalLight* directionalLightData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource_ = nullptr;

	// 使用するモデル数
	UINT modelCount_ = 0;

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_ = {};
	
};