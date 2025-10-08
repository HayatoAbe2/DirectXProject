#include "PipelineStateManager.h"
#include <cassert>

void PipelineStateManager::Initialize(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature){
	device_ = device;
	rootSignature_ = rootSignature;

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputLayoutDesc_.pInputElementDescs = inputElementDescs;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs);


	CreatePipelineState();
}

void PipelineStateManager::CreatePipelineState() {
	// ブレンド設定
	// すべての色要素を書き込む
	blendDesc_.RenderTarget[0].BlendEnable = FALSE;
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// ラスタライザ設定
	// 裏面(時計回り)を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_NONE;
	// 三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;

	// DepthStencil設定
	// Depthの機能を有効化する
	depthStencilDesc_.DepthEnable = true;
	// 書き込み
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	HRESULT hr;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();		// RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc_;		// InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob_->GetBufferPointer(),
	vertexShaderBlob_->GetBufferSize() };							// VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob_->GetBufferPointer(),
	pixelShaderBlob_->GetBufferSize() };								// PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc_;				// BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc_;		// RasterizerState
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc_;	// DepthStencilState
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成
	assert(rootSignature_);
	assert(vertexShaderBlob_);
	assert(pixelShaderBlob_);
	assert(inputLayoutDesc_.pInputElementDescs != nullptr);

	hr = device_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&noneBlendPSO));
	assert(SUCCEEDED(hr));

	//
	// スプライト用(アルファ値対応)PSO
	//

	// DepthStencil設定
	D3D12_DEPTH_STENCIL_DESC spriteDepthDesc{};
	spriteDepthDesc.DepthEnable = true;
	spriteDepthDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	spriteDepthDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// PSO 設定
	graphicsPipelineStateDesc.DepthStencilState = spriteDepthDesc;


	// αブレンド有効
	D3D12_BLEND_DESC alphaBlendDesc{};
	alphaBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	alphaBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	alphaBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	alphaBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	alphaBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	alphaBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	alphaBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	alphaBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// BlendStateのみ変更
	graphicsPipelineStateDesc.BlendState = alphaBlendDesc;
	graphicsPipelineStateDesc.DepthStencilState = spriteDepthDesc;

	hr = device_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&alphaBlendPSO_));
	assert(SUCCEEDED(hr));


	D3D12_BLEND_DESC addBlendDesc{};
	addBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	addBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	addBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

	graphicsPipelineStateDesc.BlendState = addBlendDesc;
	hr = device_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&addBlendPSO_));
	assert(SUCCEEDED(hr));

	D3D12_BLEND_DESC subtractBlendDesc{};
	subtractBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	subtractBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	subtractBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_SUBTRACT;
	graphicsPipelineStateDesc.BlendState = subtractBlendDesc;
	hr = device_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&subtractBlendPSO_));
	assert(SUCCEEDED(hr));

	D3D12_BLEND_DESC multiplyBlendDesc{};
	multiplyBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	multiplyBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	multiplyBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	graphicsPipelineStateDesc.BlendState = multiplyBlendDesc;
	hr = device_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&multiplyBlendPSO_));
	assert(SUCCEEDED(hr));

	D3D12_BLEND_DESC screenBlendDesc{};
	screenBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	screenBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	screenBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	graphicsPipelineStateDesc.BlendState = screenBlendDesc;
	hr = device_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&screenBlendPSO_));
	assert(SUCCEEDED(hr));




	//
	// グリッド用の設定
	//

	//gridVSBlob_ = shaderCompiler_->Compile(L"Grid.VS.hlsl",
	//	L"vs_6_0", logger_);
	//assert(gridVSBlob_ != nullptr);

	//gridPSBlob_ = shaderCompiler_->Compile(L"Grid.PS.hlsl",
	//	L"ps_6_0", logger_);
	//assert(gridPSBlob_ != nullptr);

	//// モデル描画用からコピー
	//gridPipelineStateDesc_ = graphicsPipelineStateDesc;

	//// トポロジタイプをラインにする
	//gridPipelineStateDesc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	//// シェーダー適用
	//gridPipelineStateDesc_.VS = { gridVSBlob_->GetBufferPointer(), gridVSBlob_->GetBufferSize() };
	//gridPipelineStateDesc_.PS = { gridPSBlob_->GetBufferPointer(), gridPSBlob_->GetBufferSize() };

	//gridPipelineStateDesc_.InputLayout = gridInputLayoutDesc_;
	//gridPipelineStateDesc_.pRootSignature = rootSignature_.Get();

	//// PipelineState作成
	//hr = deviceManager_->GetDevice()->CreateGraphicsPipelineState(&gridPipelineStateDesc_,
	//	IID_PPV_ARGS(&gridPipelineState_));
	//assert(SUCCEEDED(hr));
}
