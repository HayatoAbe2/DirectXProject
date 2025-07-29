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
class ShaderCompiler;
class DescriptorHeapManager;
class RenderTargetManager;
class Graphics {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(int32_t clientWidth, int32_t clientHeight, HWND hwnd, Logger* logger);

	/// <summary>
	/// モデル用SRV作成
	/// </summary>
	Model* CreateSRV(Model* model);

	/// <summary>
	/// スプライト用SRV作成
	/// </summary>
	Sprite* CreateSRV(Sprite* sprite);

	/// <summary>
	/// モデル描画
	/// </summary>
	/// <param name="model">描画するモデル</param>
	void DrawModel(Model& model);

	/// <summary>
	/// スプライト描画
	/// </summary>
	/// <param name="sprite">描画するスプライト</param>
	void DrawSprite(Sprite& sprite);

	/// <summary>
	/// グリッド描画
	/// </summary>
	void DrawGrid(Camera& camera);

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
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes);

	// アクセサ
	Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() const { return device_; }
	int32_t GetWindowWidth() { return clientWidth_; };
	int32_t GetWindowHeight() { return clientHeight_; };

private:

	/// <summary>
	/// 使用アダプタの取得
	/// </summary>
	void SelectAdapter();

	/// <summary>
	/// デバイス生成
	/// </summary>
	void CreateD3D12Device();

	/// <summary>
	/// エラーメッセージ等設定
	/// </summary>
	void DebugFilter();

	/// <summary>
	/// Fence作成
	/// </summary>
	void InitializeFence();

	/// <summary>
	/// CommandQueue作成
	/// </summary>
	void InitializeCommandQueue();

	/// <summary>
	/// CommandAllocator作成
	/// </summary>
	void InitializeCommandAllocator();

	/// <summary>
	/// CommandList作成
	/// </summary>
	void InitializeCommandList();

	/// <summary>
	/// SwapChain初期化
	/// </summary>
	void InitializeSwapChain(HWND hwnd);

	/// <summary>
	/// DescriptorHeap初期化,RTV設定
	/// </summary>
	void InitializeDescriptorHeaps();

	/// <summary>
	/// RootSignature,RootParameter,Sampler設定
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// Blend,Rasterizer,DepthStencil設定
	/// </summary>
	void CreatePipelineState();

	/// <summary>
	/// Viewport,Scissor設定
	/// </summary>
	void SetViewportAndScissor();

	/// <summary>
	/// ImGui初期化
	/// </summary>
	void InitializeImGui(HWND hwnd);

	/// <summary>
	/// DirectionalLight初期化
	/// </summary>
	void CreateLightBuffer();

	/// <summary>
	/// Texture読み込み
	/// </summary>
	DirectX::ScratchImage LoadTexture(const std::string& filePath);

	/// <summary>
	/// Resource作成
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const DirectX::TexMetadata& metadata);

	/// <summary>
	/// Texture転送
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages, const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList);

	/// <summary>
	/// DescriptorHandle取得
	/// </summary>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
		const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap,
		uint32_t descriptorSize, uint32_t index);

	/// <summary>
	/// TextureResource作成
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(
		const Microsoft::WRL::ComPtr<ID3D12Device>& device, int32_t width, int32_t height);

	/// <summary>
	/// グリッド初期化
	/// </summary>
	void InitializeGrid();


	// 画面サイズ
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
	
	UINT backBufferIndex_;


	// 深度ステンシルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;

	// 遷移バリア
	D3D12_RESOURCE_BARRIER barrier_ = {};

	// inputLayout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_;

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

	TransformationMatrix* transformationMatrixData_ = nullptr;
	TransformationMatrix* transformationMatrixDataSprite_ = nullptr;

	D3D12_VIEWPORT viewport_ = {};
	D3D12_RECT scissorRect_ = {};

	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceSprite_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;
	
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_ = nullptr;
	DirectionalLight* directionalLightData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> gridTextureResource_ = nullptr;

	// 使用するモデル数
	UINT modelCount_ = 0;

	// グリッドで使用
	Microsoft::WRL::ComPtr<ID3D12PipelineState> gridPipelineState_;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> gridRootSignature_;
	Microsoft::WRL::ComPtr<ID3D12Resource> gridMaterialResource_;
	Material* gridMaterialData_;

	std::vector<VertexData> gridVertices_;
	Microsoft::WRL::ComPtr<ID3D12Resource> gridVertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW gridVBV_;
	Material gridMaterial_;

	// グリッド原点
	std::vector<VertexData> gridVerticesOrigin_;
	D3D12_VERTEX_BUFFER_VIEW gridVBVOrigin_;
	Microsoft::WRL::ComPtr<ID3D12Resource> gridVertexBufferOrigin_;
	Microsoft::WRL::ComPtr<ID3D12Resource> gridMaterialResourceOrigin_;
	Material* gridMaterialDataOrigin_;
	Material gridMaterialOrigin_;

	// グリッド強調線
	std::vector<VertexData> gridVerticesMark_;
	D3D12_VERTEX_BUFFER_VIEW gridVBVMark_;
	Microsoft::WRL::ComPtr<ID3D12Resource> gridVertexBufferMark_;
	Microsoft::WRL::ComPtr<ID3D12Resource> gridMaterialResourceMark_;
	Material* gridMaterialDataMark_;
	Material gridMaterialMark_;

	Microsoft::WRL::ComPtr<ID3D12Resource> gridTransformationResource_;
	TransformationMatrix* gridTransformationData_;
	Transform gridTransform_;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gridPipelineStateDesc_;
	D3D12_INPUT_LAYOUT_DESC gridInputLayoutDesc_;
	D3D12_GPU_DESCRIPTOR_HANDLE gridSRVHandleGPU_;

	// グリッド用シェーダー
	Microsoft::WRL::ComPtr<IDxcBlob> gridVSBlob_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> gridPSBlob_ = nullptr;

	// SRVのインデックス
	UINT currentSRVIndex_ = 0;

	// ログクラス
	Logger* logger_;

	// シェーダーコンパイルクラス
	ShaderCompiler* shaderCompiler_ = nullptr;

	// ディスクリプタヒープ管理クラス
	DescriptorHeapManager* descriptorHeapManager_ = nullptr;

	// 
	RenderTargetManager* renderTargetManager_ = nullptr;
};