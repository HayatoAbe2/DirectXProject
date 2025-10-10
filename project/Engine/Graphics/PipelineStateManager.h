#pragma once

#include "BlendMode.h"

#include <d3d12.h>
#include <wrl.h>
#include <dxcapi.h>
class PipelineStateManager {
public:

	void Initialize(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature);

	void CreatePipelineState();

	ID3D12PipelineState* GetPSO(int index) { return pso_[index].Get(); }

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
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pso_[7]{};

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

	D3D12_BLEND_DESC CreateNoneBlendDesc();
	D3D12_BLEND_DESC CreateAlphaBlendDesc();
	D3D12_BLEND_DESC CreateAddBlendDesc();
	D3D12_BLEND_DESC CreateSubtractBlendDesc();
	D3D12_BLEND_DESC CreateMultiplyBlendDesc();
	D3D12_BLEND_DESC CreateScreenBlendDesc();

	D3D12_DEPTH_STENCIL_DESC CreateDefaultDepthDesc();
	void CreatePSO(D3D12_GRAPHICS_PIPELINE_STATE_DESC baseDesc, const D3D12_BLEND_DESC& blendDesc, Microsoft::WRL::ComPtr<ID3D12PipelineState>* outPSO);

};

