#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <dxcapi.h>
class PipelineStateManager {
public:

	void Initialize(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature);

	void CreatePipelineState();

	ID3D12PipelineState* GetNoneBlendPSO() { return noneBlendPSO.Get(); }
	ID3D12PipelineState* GetAlphaBlendPSO() { return alphaBlendPSO_.Get(); }

	void SetVSBlob(Microsoft::WRL::ComPtr<IDxcBlob> blob) { vertexShaderBlob_ = blob; }
	void SetPSBlob(Microsoft::WRL::ComPtr<IDxcBlob> blob) { pixelShaderBlob_ = blob; }

private:

	// inputLayout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_;

	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc_{};

	// ResterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc_ = {};

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};

	// PSO
	Microsoft::WRL::ComPtr<ID3D12PipelineState> noneBlendPSO;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> alphaBlendPSO_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> addBlendPSO_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> subtractBlendPSO_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> multiplyBlendPSO_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> screenBlendPSO_;

	//
	// 参照
	//
	
	// デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;

	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

	// シェーダー
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;

};

