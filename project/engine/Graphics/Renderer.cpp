#include <Windows.h>
#include "Renderer.h"
#include "DeviceManager.h"
#include "CommandListManager.h"
#include "RootSignatureManager.h"
#include "ShaderCompiler.h"
#include "RenderTargetManager.h"
#include "DescriptorHeapManager.h"
#include "PipelineStateManager.h"
#include "../Object/Sprite.h"
#include "../Object/ResourceManager.h"

#include <cassert>
#include <format>
#include <dxcapi.h>
#include <mfobjects.h>
#include <numbers>

#include "externals/DirectXTex/d3dx12.h"
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

void Renderer::Initialize(int32_t clientWidth, int32_t clientHeight, HWND hwnd, Logger* logger) {
	HRESULT hr;

	clientWidth_ = clientWidth;
	clientHeight_ = clientHeight;
	logger_ = logger;

	// DXGIファクトリーの生成
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(hr));

	// デバイスマネージャー初期化
	deviceManager_ = new DeviceManager;
	deviceManager_->Initialize(dxgiFactory_.Get(), logger_);

	// コマンドリストマネージャー初期化
	commandListManager_ = new CommandListManager;
	commandListManager_->Initialize(deviceManager_);

	// スワップチェーンの生成
	InitializeSwapChain(hwnd);

	// ディスクリプタヒープの初期化
	descriptorHeapManager_ = new DescriptorHeapManager;
	descriptorHeapManager_->Initialize(deviceManager_->GetDevice().Get());

	// RTV作成
	renderTargetManager_ = new RenderTargetManager;
	renderTargetManager_->InitializeSwapChainBuffers(swapChain_.Get(), deviceManager_->GetDevice().Get(), descriptorHeapManager_);

	// DepthStencilTextureをウィンドウのサイズで作成
	depthStencilResource_ = CreateDepthStencilTextureResource(deviceManager_->GetDevice(), clientWidth_, clientHeight_);
	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2DTexture
	// DSVHeapの先頭にDSVを作る
	deviceManager_->GetDevice()->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, descriptorHeapManager_->GetDSVHeap()->GetCPUDescriptorHandleForHeapStart());

	// ルートシグネチャマネージャー
	rootSignatureManager_ = new RootSignatureManager;
	rootSignatureManager_->Initialize(deviceManager_->GetDevice(), logger_);

	shaderCompiler_ = new ShaderCompiler;
	shaderCompiler_->Initialize();

	// Shaderをコンパイルする
	pipelineStateManager_ = new PipelineStateManager;
	pipelineStateManager_->SetVSBlob(shaderCompiler_->Compile(L"Resources/shaders/Object3D.VS.hlsl", L"vs_6_0", logger_));
	pipelineStateManager_->SetPSBlob(shaderCompiler_->Compile(L"Resources/shaders/Object3D.PS.hlsl", L"ps_6_0", logger_));
	
	pipelineStateManager_->SetInstancingVSBlob(shaderCompiler_->Compile(L"Resources/shaders/Particle.VS.hlsl", L"vs_6_0", logger_));
	pipelineStateManager_->SetInstancingPSBlob(shaderCompiler_->Compile(L"Resources/shaders/Particle.PS.hlsl", L"ps_6_0", logger_));

	// コンパイラ解放
	delete shaderCompiler_;

	// PSOマネージャー
	pipelineStateManager_->Initialize(deviceManager_->GetDevice(), rootSignatureManager_->GetStandardRootSignature(),rootSignatureManager_->GetInstancingRootSignature());

	CreateLightBuffer();

	SetViewportAndScissor();
}

void Renderer::DrawModel(Model& model,int blendMode) {

	model.UpdateMaterial();

	// PSO設定
	commandListManager_->GetCommandList()->SetPipelineState(pipelineStateManager_->GetStandardPSO(blendMode));
	// RootSignatureを設定
	commandListManager_->GetCommandList()->SetGraphicsRootSignature(rootSignatureManager_->GetStandardRootSignature().Get());
	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeapManager_->GetSRVHeap().Get() };
	commandListManager_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);

	// トポロジを三角形に設定
	commandListManager_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルCBufferの場所を設定
	commandListManager_->GetCommandList()->SetGraphicsRootConstantBufferView(0, model.GetMaterialCBV());
	// モデル描画
	commandListManager_->GetCommandList()->IASetVertexBuffers(0, 1, &model.GetVBV());	// VBVを設定
	// wvp用のCBufferの場所を設定
	commandListManager_->GetCommandList()->SetGraphicsRootConstantBufferView(1, model.GetCBV());
	// SRVの設定
	if (model.GetTextureSRVHandle().ptr != 0) {
		commandListManager_->GetCommandList()->SetGraphicsRootDescriptorTable(2, model.GetTextureSRVHandle());
	}
	// ライト
	commandListManager_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	// ドローコール
	commandListManager_->GetCommandList()->DrawInstanced(UINT(model.GetVertices().size()), 1, 0, 0);
}

void Renderer::DrawModelInstance(Model& model, int blendMode) {

	model.UpdateMaterial();
	
	// PSO設定
	commandListManager_->GetCommandList()->SetPipelineState(pipelineStateManager_->GetInstancingPSO(blendMode));
	// RootSignatureを設定
	commandListManager_->GetCommandList()->SetGraphicsRootSignature(rootSignatureManager_->GetInstancingRootSignature().Get());
	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeapManager_->GetSRVHeap().Get() };
	commandListManager_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);

	// トポロジを三角形に設定
	commandListManager_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルCBufferの場所を設定
	commandListManager_->GetCommandList()->SetGraphicsRootConstantBufferView(0, model.GetMaterialCBV());
	// モデル描画
	commandListManager_->GetCommandList()->IASetVertexBuffers(0, 1, &model.GetVBV());	// VBVを設定
	// wvp用のCBufferの場所を設定
	commandListManager_->GetCommandList()->SetGraphicsRootConstantBufferView(1, model.GetInstanceCBV());
	// SRVの設定
	if (model.GetTextureSRVHandle().ptr != 0) {
	commandListManager_->GetCommandList()->SetGraphicsRootDescriptorTable(2, model.GetTextureSRVHandle());
	}
	// インスタンス用SRVの設定
	if (model.IsInstancing()) {
		commandListManager_->GetCommandList()->SetGraphicsRootDescriptorTable(4, model.GetInstanceSRVHandle());
	}
	// ドローコール
	commandListManager_->GetCommandList()->DrawInstanced(UINT(model.GetVertices().size()), model.GetNumInstance(), 0, 0);
}

void Renderer::DrawSprite(Sprite& sprite,int blendMode) {
	// PSO設定
	commandListManager_->GetCommandList()->SetPipelineState(pipelineStateManager_->GetStandardPSO(blendMode));
	// RootSignatureを設定
	commandListManager_->GetCommandList()->SetGraphicsRootSignature(rootSignatureManager_->GetStandardRootSignature().Get());
	// トポロジを三角形に設定
	commandListManager_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルCBufferの場所を設定
	commandListManager_->GetCommandList()->SetGraphicsRootConstantBufferView(0, sprite.GetCBV());
	// Spriteの描画。変更が必要なものだけ変更する
	commandListManager_->GetCommandList()->IASetIndexBuffer(&sprite.GetIBV());	// IBVを設定
	commandListManager_->GetCommandList()->IASetVertexBuffers(0, 1, &sprite.GetVBV());	// VBVを設定
	// TransformationMatrixCBufferの場所を設定
	commandListManager_->GetCommandList()->SetGraphicsRootConstantBufferView(1, sprite.GetCBV());
	// SRVの設定
	commandListManager_->GetCommandList()->SetGraphicsRootDescriptorTable(2, sprite.GetTextureSRVHandle());
	// ライト
	commandListManager_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	// 描画!(DrawCall/ドローコール)
	commandListManager_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Renderer::Finalize() {
	if (rootSignatureManager_->GetErrorBlob()) rootSignatureManager_->GetErrorBlob()->Release();
	delete deviceManager_;
	delete commandListManager_;
	delete descriptorHeapManager_;
	delete renderTargetManager_;
	delete rootSignatureManager_;
	delete pipelineStateManager_;
}

void Renderer::BeginFrame() {


	// これから書き込むバックバッファのインデックスを取得
	backBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();
	// TransitionBarrierの設定
	// バリアはTransition
	barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バリアを張る対象のリソース。現存のバックバッファに対して行う
	barrier_.Transition.pResource = renderTargetManager_->GetSwapChainResource(backBufferIndex_).Get();
	// 遷移前(現存)のResourceState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	// 遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	// TransitionBarrierを張る
	assert(renderTargetManager_->GetSwapChainResource(backBufferIndex_));

	commandListManager_->GetCommandList()->ResourceBarrier(1, &barrier_);


	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = renderTargetManager_->GetCPUDescriptorHandle(descriptorHeapManager_->GetDSVHeap().Get(), descriptorHeapManager_->GetDSVHeapSize(), 0);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderTargetManager_->GetRTVHandle(backBufferIndex_);
	commandListManager_->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	// 指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f }; // 青っぽい色。RGBAの順
	commandListManager_->GetCommandList()->ClearRenderTargetView(renderTargetManager_->GetRTVHandle(backBufferIndex_), clearColor, 0, nullptr);
	// 指定した深度で画面全体をクリアする
	commandListManager_->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeapManager_->GetSRVHeap().Get() };
	commandListManager_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
	// Viewportを設定
	commandListManager_->GetCommandList()->RSSetViewports(1, &viewport_);
	// Scissorを設定
	commandListManager_->GetCommandList()->RSSetScissorRects(1, &scissorRect_);
}

void Renderer::EndFrame() {
	HRESULT hr;

	//// ImGuiの内部コマンドを生成する
	//ImGui::Render();

	//// 実際のcommandListのImGuiの描画コマンドを読む
	//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandListManager_->GetCommandList().Get());

	// 今回はRenderTargetからPresentにする
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	// TransitionBarrierを張る
	commandListManager_->GetCommandList()->ResourceBarrier(1, &barrier_);

	// コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
	hr = commandListManager_->GetCommandList()->Close();
	assert(SUCCEEDED(hr));

	// GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { commandListManager_->GetCommandList().Get() };
	commandListManager_->GetCommandQueue()->ExecuteCommandLists(1, commandLists);
	// GPUとOSに画面の交換を行うよう通知する
	swapChain_->Present(1, 0);

	commandListManager_->Wait();
	commandListManager_->Reset();
}

void Renderer::InitializeSwapChain(HWND hwnd) {
	HRESULT hr;
	swapChainDesc_.Width = clientWidth_;								// 画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc_.Height = clientHeight_;							// 画面の高さ。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;				// 色の形式
	swapChainDesc_.SampleDesc.Count = 1;							// マルチサンプルしない
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 描画のターゲットとして利用する
	swapChainDesc_.BufferCount = 2;									// ダブルバッファ
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		// モニタにうつしたら、中身を破棄
	// コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory_->CreateSwapChainForHwnd(commandListManager_->GetCommandQueue().Get(), hwnd, &swapChainDesc_, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr));
}

Microsoft::WRL::ComPtr<ID3D12Resource> Renderer::CreateDepthStencilTextureResource(
	const Microsoft::WRL::ComPtr<ID3D12Device>& device, int32_t width, int32_t height) {

	// 生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;										// Textureの幅
	resourceDesc.Height = height;									// Textureの高さ
	resourceDesc.MipLevels = 1;										// mipmapの数
	resourceDesc.DepthOrArraySize = 1;								// 奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;			// DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;								// サンプル数(1固定)
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	// 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	// DepthStencilとして使う通知

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;	// VRAM上に作る

	// 深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;	// 1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// フォーマット。Resourceと合わせる

	// Resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,					// Heapの設定
		D3D12_HEAP_FLAG_NONE,				// Heapの特殊な設定。特になし。	
		&resourceDesc,						// Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	// 深度値を書き込む状態にしておく
		&depthClearValue,					// Clear最適値
		IID_PPV_ARGS(&resource_));			// 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));

	return resource_;
}

Microsoft::WRL::ComPtr<ID3D12Resource> Renderer::CreateBufferResource(size_t sizeInBytes) {
	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;		// uploadHeapを使う
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Alignment = 0;
	vertexResourceDesc.Width = sizeInBytes;					// リソースのサイズ
	// バッファの場合はこれらは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際にリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	HRESULT hr = deviceManager_->GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));
	return vertexResource;
}

void Renderer::SetViewportAndScissor() {
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = float(clientWidth_);
	viewport_.Height = float(clientHeight_);
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	// シザー矩形
	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect_.left = 0;
	scissorRect_.right = LONG(clientWidth_);
	scissorRect_.top = 0;
	scissorRect_.bottom = LONG(clientHeight_);
}

void Renderer::CreateLightBuffer() {
	// DirectionalLight用のResource
	directionalLightResource_ = CreateBufferResource(sizeof(DirectionalLight));
	// 書き込むためのアドレスを取得
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	directionalLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightData_->direction = Normalize({ 0.2f, -0.6f, 1.5f });
	directionalLightData_->intensity = 1.0f;
	directionalLightData_->lightingType = 1;
}