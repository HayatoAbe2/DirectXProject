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
#include "ModelData.h"
#include "DictionalLight.h"


class Graphics {
private:

	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};
	
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	struct MaterialData {
		std::string textureFilePath;
	};

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(int32_t clientWidth, int32_t clientHeight, HWND hwnd);

	// 更新処理
	void BeginImGuiFrame();
	void EndImGuiFrame(HWND hwnd);
	void RenderImGui();

	void UpdateCamera(const Transform& cameraTransform, DebugCamera& debugCamera);
	void UpdateModel(UINT modelIndex, const Transform& modelTransform);
	void UpdateMaterial(const Material& mat);
	void UpdateSprite(const Transform& spriteTransform, const Transform& uvTransform, const Transform& cameraTransform);

	// 描画処理
	void prepareDraw();
	void DrawModel(ModelData model);
	void DrawSprite();

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

	uint32_t LoadTextureAndCreateSRV(const std::string& path);

	void CreateTransformBuffers();

	void CreateLightBuffer();

	void CreateSRV(const std::string& filePath);

	DirectX::ScratchImage LoadTexture(const std::string& filePath);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const DirectX::TexMetadata& metadata);

	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages, const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes);



	Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() const {
		return device_;
	}

private:
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
	// スワップチェーンのリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2] = { nullptr, nullptr };
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	UINT backBufferIndex_;
	// 深度ステンシルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;
	// 遷移バリア
	D3D12_RESOURCE_BARRIER barrier_ = {};
	// inputLayout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_;

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
	IDxcBlob* vertexShaderBlob_ = nullptr;
	IDxcBlob* pixelShaderBlob_ = nullptr;

	ID3DBlob* signatureBlob_ = nullptr;
	ID3DBlob* errorBlob_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

	// ログ
	Logger logger_;

	Material* materialData_ = nullptr;
	Material* spriteMaterialData_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSprite_ = nullptr;
	Material* materialDataSprite_ = nullptr;

	TransformationMatrix* transformationMatrixData_ = nullptr;
	TransformationMatrix* transformationMatrixDataSprite_ = nullptr;

	Matrix4x4 viewMatrix_;
	Matrix4x4 projectionMatrix_;
	std::vector< TransformationMatrix* > transformationData_;

	D3D12_VIEWPORT viewport_ = {};
	D3D12_RECT scissorRect_ = {};
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceSprite_ = nullptr;
	ID3D12PipelineState* graphicsPipelineState_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_ = nullptr;
	DirectionalLight* directionalLightData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource_ = nullptr;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> transformationResource_;

	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_ = {};
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_ = {};

	UINT modelCount_ = 0;
};