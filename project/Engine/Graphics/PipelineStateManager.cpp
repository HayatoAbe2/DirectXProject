#include "PipelineStateManager.h"
#include <cassert>

void PipelineStateManager::Initialize(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature,
	const Microsoft::WRL::ComPtr<ID3D12RootSignature>& instancingRootSignature, const Microsoft::WRL::ComPtr<ID3D12RootSignature>& particleRootSignature){
	device_ = device;
	rootSignature_ = rootSignature;
	instancingRootSignature_ = instancingRootSignature;
	particleRootSignature_ = particleRootSignature;

	// InputLayout
	inputElementDescs_[0].SemanticName = "POSITION";
	inputElementDescs_[0].SemanticIndex = 0;
	inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs_[1].SemanticName = "TEXCOORD";
	inputElementDescs_[1].SemanticIndex = 0;
	inputElementDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs_[2].SemanticName = "NORMAL";
	inputElementDescs_[2].SemanticIndex = 0;
	inputElementDescs_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs_[3].SemanticName = "COLOR";
	inputElementDescs_[3].SemanticIndex = 0;
	inputElementDescs_[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs_[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs_);


	CreateStandardPSO();
	CreateInstancingPSO();
	CreateParticlePSO();
}

void PipelineStateManager::CreateStandardPSO() {
	assert(rootSignature_);
	assert(vertexShaderBlob_);
	assert(pixelShaderBlob_);
	assert(inputLayoutDesc_.pInputElementDescs != nullptr);

	// 共通部分作成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC baseDesc{};
	baseDesc.pRootSignature = rootSignature_.Get();
	baseDesc.InputLayout = inputLayoutDesc_;
	baseDesc.VS = { vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize() };
	baseDesc.PS = { pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize() };

	// ブレンド
	baseDesc.BlendState = CreateNoneBlendDesc();

	// ラスタライザ
	baseDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	baseDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStencil
	baseDesc.DepthStencilState.DepthEnable = TRUE;
	baseDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	baseDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	baseDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	baseDesc.NumRenderTargets = 1;
	baseDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	baseDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	baseDesc.SampleDesc.Count = 1;
	baseDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// --- 各ブレンドモードごとのPSO生成 ---
	CreatePSO(baseDesc, CreateNoneBlendDesc(), &pso_[static_cast<int>(BlendMode::None)]);			// ブレンドなし
	CreatePSO(baseDesc, CreateAlphaBlendDesc(), &pso_[static_cast<int>(BlendMode::Normal)]);		// αブレンド
	CreatePSO(baseDesc, CreateAddBlendDesc(), &pso_[static_cast<int>(BlendMode::Add)]);				// 加算
	CreatePSO(baseDesc, CreateSubtractBlendDesc(), &pso_[static_cast<int>(BlendMode::Subtract)]);	// 減算
	CreatePSO(baseDesc, CreateMultiplyBlendDesc(), &pso_[static_cast<int>(BlendMode::Multiply)]);	// 乗算
	CreatePSO(baseDesc, CreateScreenBlendDesc(), &pso_[static_cast<int>(BlendMode::Screen)]);		// スクリーン
}

void PipelineStateManager::CreateInstancingPSO() {
	assert(instancingRootSignature_);
	assert(instancingVertexShaderBlob_);
	assert(instancingPixelShaderBlob_);
	assert(inputLayoutDesc_.pInputElementDescs != nullptr);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC baseDesc{};
	baseDesc.pRootSignature = instancingRootSignature_.Get();
	baseDesc.InputLayout = inputLayoutDesc_;
	baseDesc.VS = { instancingVertexShaderBlob_->GetBufferPointer(), instancingVertexShaderBlob_->GetBufferSize() };
	baseDesc.PS = { instancingPixelShaderBlob_->GetBufferPointer(),	instancingPixelShaderBlob_->GetBufferSize()	};
	baseDesc.BlendState = CreateNoneBlendDesc();
	baseDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	baseDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	baseDesc.DepthStencilState.DepthEnable = TRUE;
	baseDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	baseDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	baseDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	baseDesc.NumRenderTargets = 1;
	baseDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	baseDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	baseDesc.SampleDesc.Count = 1;
	baseDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	CreatePSO(baseDesc, CreateNoneBlendDesc(), &instancingPso_[static_cast<int>(BlendMode::None)]);			// ブレンドなし
	CreatePSO(baseDesc, CreateAlphaBlendDesc(), &instancingPso_[static_cast<int>(BlendMode::Normal)]);		// αブレンド
	CreatePSO(baseDesc, CreateAddBlendDesc(), &instancingPso_[static_cast<int>(BlendMode::Add)]);			// 加算
	CreatePSO(baseDesc, CreateSubtractBlendDesc(), &instancingPso_[static_cast<int>(BlendMode::Subtract)]);	// 減算
	CreatePSO(baseDesc, CreateMultiplyBlendDesc(), &instancingPso_[static_cast<int>(BlendMode::Multiply)]);	// 乗算
	CreatePSO(baseDesc, CreateScreenBlendDesc(), &instancingPso_[static_cast<int>(BlendMode::Screen)]);		// スクリーン
}

void PipelineStateManager::CreateParticlePSO() {
	assert(particleRootSignature_);
	assert(particleVertexShaderBlob_);
	assert(particlePixelShaderBlob_);
	assert(inputLayoutDesc_.pInputElementDescs != nullptr);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC baseDesc{};
	baseDesc.pRootSignature = particleRootSignature_.Get();
	baseDesc.InputLayout = inputLayoutDesc_;
	baseDesc.VS = { particleVertexShaderBlob_->GetBufferPointer(), particleVertexShaderBlob_->GetBufferSize() };
	baseDesc.PS = { particlePixelShaderBlob_->GetBufferPointer(),	particlePixelShaderBlob_->GetBufferSize() };
	baseDesc.BlendState = CreateNoneBlendDesc();
	baseDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	baseDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	baseDesc.DepthStencilState.DepthEnable = TRUE;
	baseDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	baseDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	baseDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	baseDesc.NumRenderTargets = 1;
	baseDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	baseDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	baseDesc.SampleDesc.Count = 1;
	baseDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	CreatePSO(baseDesc, CreateNoneBlendDesc(), &particlePso_[static_cast<int>(BlendMode::None)]);			// ブレンドなし
	CreatePSO(baseDesc, CreateAlphaBlendDesc(), &particlePso_[static_cast<int>(BlendMode::Normal)]);		// αブレンド
	CreatePSO(baseDesc, CreateAddBlendDesc(), &particlePso_[static_cast<int>(BlendMode::Add)]);				// 加算
	CreatePSO(baseDesc, CreateSubtractBlendDesc(), &particlePso_[static_cast<int>(BlendMode::Subtract)]);	// 減算
	CreatePSO(baseDesc, CreateMultiplyBlendDesc(), &particlePso_[static_cast<int>(BlendMode::Multiply)]);	// 乗算
	CreatePSO(baseDesc, CreateScreenBlendDesc(), &particlePso_[static_cast<int>(BlendMode::Screen)]);		// スクリーン
}

// ----------------------------------------------------
// 各ブレンド設定生成
// ----------------------------------------------------

D3D12_BLEND_DESC PipelineStateManager::CreateNoneBlendDesc() {
	D3D12_BLEND_DESC desc{};
	desc.RenderTarget[0].BlendEnable = FALSE;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return desc;
}

D3D12_BLEND_DESC PipelineStateManager::CreateAlphaBlendDesc() {
	D3D12_BLEND_DESC desc{};
	auto& rt = desc.RenderTarget[0];
	rt.BlendEnable = TRUE;
	rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	rt.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	rt.BlendOp = D3D12_BLEND_OP_ADD;
	rt.SrcBlendAlpha = D3D12_BLEND_ONE;
	rt.DestBlendAlpha = D3D12_BLEND_ZERO;
	rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	rt.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return desc;
}

D3D12_BLEND_DESC PipelineStateManager::CreateAddBlendDesc() {
	D3D12_BLEND_DESC desc{};
	auto& rt = desc.RenderTarget[0];
	rt.BlendEnable = TRUE;
	rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	rt.DestBlend = D3D12_BLEND_ONE;
	rt.BlendOp = D3D12_BLEND_OP_ADD;
	rt.SrcBlendAlpha = D3D12_BLEND_ONE;
	rt.DestBlendAlpha = D3D12_BLEND_ZERO;
	rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	rt.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return desc;
}

D3D12_BLEND_DESC PipelineStateManager::CreateSubtractBlendDesc() {
	D3D12_BLEND_DESC desc{};
	auto& rt = desc.RenderTarget[0];
	rt.BlendEnable = TRUE;
	rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	rt.DestBlend = D3D12_BLEND_ONE;
	rt.BlendOp = D3D12_BLEND_OP_SUBTRACT;
	rt.SrcBlendAlpha = D3D12_BLEND_ONE;
	rt.DestBlendAlpha = D3D12_BLEND_ZERO;
	rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	rt.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return desc;
}

D3D12_BLEND_DESC PipelineStateManager::CreateMultiplyBlendDesc() {
	D3D12_BLEND_DESC desc{};
	auto& rt = desc.RenderTarget[0];
	rt.BlendEnable = TRUE;
	rt.SrcBlend = D3D12_BLEND_ZERO;
	rt.DestBlend = D3D12_BLEND_SRC_COLOR;
	rt.BlendOp = D3D12_BLEND_OP_ADD;
	rt.SrcBlendAlpha = D3D12_BLEND_ONE;
	rt.DestBlendAlpha = D3D12_BLEND_ZERO;
	rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	rt.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return desc;
}

D3D12_BLEND_DESC PipelineStateManager::CreateScreenBlendDesc() {
	D3D12_BLEND_DESC desc{};
	auto& rt = desc.RenderTarget[0];
	rt.BlendEnable = TRUE;
	rt.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	rt.DestBlend = D3D12_BLEND_ONE;
	rt.BlendOp = D3D12_BLEND_OP_ADD;
	rt.SrcBlendAlpha = D3D12_BLEND_ONE;
	rt.DestBlendAlpha = D3D12_BLEND_ZERO;
	rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	rt.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return desc;
}

// ----------------------------------------------------
// PSO生成
// ----------------------------------------------------

void PipelineStateManager::CreatePSO(
	D3D12_GRAPHICS_PIPELINE_STATE_DESC& baseDesc,
	const D3D12_BLEND_DESC& blendDesc,
	Microsoft::WRL::ComPtr<ID3D12PipelineState>* outPSO) {

	baseDesc.BlendState = blendDesc;
	HRESULT hr = device_->CreateGraphicsPipelineState(&baseDesc, IID_PPV_ARGS(outPSO->ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(hr));
}
