#include "Windows.h"
#include "Graphics.h"
#include <cassert>
#include <format>
#include <dxcapi.h>
#include "externals/DirectXTex/d3dx12.h"
#include <mfobjects.h>
#include "Sprite.h"
#include "ShaderCompiler.h"
#include "DescriptorHeapManager.h"
#include "RenderTargetManager.h"

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include <numbers>

void Graphics::Initialize(int32_t clientWidth, int32_t clientHeight, HWND hwnd, Logger* logger) {
	HRESULT hr;

	clientWidth_ = clientWidth;
	clientHeight_ = clientHeight;
	logger_ = logger;

	// DXGIファクトリーの生成
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(hr));
	// 使用するアダプタを選択
	SelectAdapter();
	// D3D12デバイスの生成
	CreateD3D12Device();

#ifdef _DEBUG
	// デバッグレイヤーのフィルタリング
	DebugFilter();
#endif

	// フェンス生成
	InitializeFence();
	// コマンドキュー生成
	InitializeCommandQueue();
	// コマンドアロケータ生成
	InitializeCommandAllocator();
	// コマンドリスト生成
	InitializeCommandList();

	// スワップチェーンの生成
	InitializeSwapChain(hwnd);

	// ディスクリプタヒープの初期化
	descriptorHeapManager_ = new DescriptorHeapManager;
	descriptorHeapManager_->Initialize(device_.Get());

	// RTV作成
	renderTargetManager_ = new RenderTargetManager;
	renderTargetManager_->InitializeSwapChainBuffers(swapChain_.Get(), device_.Get(), descriptorHeapManager_);

	// DepthStencilTextureをウィンドウのサイズで作成
	depthStencilResource_ = CreateDepthStencilTextureResource(device_, clientWidth_, clientHeight_);
	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2DTexture
	// DSVHeapの先頭にDSVを作る
	device_->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, descriptorHeapManager_->GetDSVHeap()->GetCPUDescriptorHandleForHeapStart());

	CreateRootSignature();
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

	D3D12_INPUT_ELEMENT_DESC gridInputElements[] = {
	{
		"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}
	};
	gridInputLayoutDesc_.pInputElementDescs = gridInputElements;
	gridInputLayoutDesc_.NumElements = _countof(gridInputElements);

	shaderCompiler_ = new ShaderCompiler;
	shaderCompiler_->Initialize();

	// Shaderをコンパイルする
	vertexShaderBlob_ = shaderCompiler_->Compile(L"Object3D.VS.hlsl",
		L"vs_6_0", logger_);
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = shaderCompiler_->Compile(L"Object3D.PS.hlsl",
		L"ps_6_0", logger_);
	assert(pixelShaderBlob_ != nullptr);

	CreatePipelineState();

	// コンパイラ解放
	delete shaderCompiler_;

	CreateLightBuffer();

	currentSRVIndex_ = 1;

	SetViewportAndScissor();

	InitializeImGui(hwnd);

	// 図形描画のための初期化
	InitializeGrid();
	InitializeSphere();
}

void Graphics::DrawModel(Model& model) {
	// PSO設定
	commandList_->SetPipelineState(graphicsPipelineState_.Get());
	// トポロジを三角形に設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアルCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, model.GetMaterialAddress());

	// モデル描画
	commandList_->IASetVertexBuffers(0, 1, &model.GetVBV());	// VBVを設定

	// wvp用のCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(1, model.GetCBV());

	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]。
	commandList_->SetGraphicsRootDescriptorTable(2, model.GetTextureSRVHandle());

	// ライト
	commandList_->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	// ドローコール
	commandList_->DrawInstanced(UINT(model.GetVertices().size()), 1, 0, 0);
}

void Graphics::DrawSprite(Sprite& sprite) {
	// PSO設定
	commandList_->SetPipelineState(graphicsPipelineState_.Get());
	// トポロジを三角形に設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// マテリアルCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, sprite.GetMaterialAddress());

	// Spriteの描画。変更が必要なものだけ変更する
	commandList_->IASetIndexBuffer(&sprite.GetIBV());	// IBVを設定
	commandList_->IASetVertexBuffers(0, 1, &sprite.GetVBV());	// VBVを設定

	// TransformationMatrixCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(1, sprite.GetCBV());

	// SRVの設定
	commandList_->SetGraphicsRootDescriptorTable(2, sprite.GetTextureSRVHandle());

	// ライト
	commandList_->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	// 描画!(DrawCall/ドローコール)
	commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Graphics::Finalize() {
	// ImGuiの終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (errorBlob_) errorBlob_->Release();
	delete descriptorHeapManager_;
	delete renderTargetManager_;
}

void Graphics::BeginFrame() {


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

	commandList_->ResourceBarrier(1, &barrier_);


	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = GetCPUDescriptorHandle(descriptorHeapManager_->GetDSVHeap(), descriptorHeapManager_->GetDSVHeapSize(), 0);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderTargetManager_->GetRTVHandle(backBufferIndex_);
	commandList_->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	// 指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f }; // 青っぽい色。RGBAの順
	commandList_->ClearRenderTargetView(renderTargetManager_->GetRTVHandle(backBufferIndex_), clearColor, 0, nullptr);
	// 指定した深度で画面全体をクリアする
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeapManager_->GetSRVHeap().Get() };
	commandList_->SetDescriptorHeaps(1, descriptorHeaps);


	commandList_->RSSetViewports(1, &viewport_);					// Viewportを設定
	commandList_->RSSetScissorRects(1, &scissorRect_);			// Scissorを設定
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());

	// ImGuiフレーム
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Graphics::EndFrame() {
	HRESULT hr;

	// ImGuiの内部コマンドを生成する
	ImGui::Render();

	// 実際のcommandListのImGuiの描画コマンドを読む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList_.Get());

	// 今回はRenderTargetからPresentにする
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	// TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier_);

	// コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
	hr = commandList_->Close();
	assert(SUCCEEDED(hr));

	// GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);
	// GPUとOSに画面の交換を行うよう通知する
	swapChain_->Present(1, 0);


	// Fenceの値を更新
	fenceValue_++;
	// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceValue_);
	// Fenceの値が指定したSignal値にたどり着いているか確認する
	// GetCompletedVallueの初期値はFence作成時に渡した初期値
	if (fence_->GetCompletedValue() < fenceValue_) {
		// 指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		// イベント待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	// 次のフレーム用のコマンドリストを準備
	hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));
}


void Graphics::SelectAdapter() {
	HRESULT hr;
	// 使用アダプタの取得
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) != // 最も良い順
		DXGI_ERROR_NOT_FOUND; ++i) {
		// アダプタの情報を取得
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter_->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr)); // 取得に成功したかどうか判定

		// ソフトウェアアダプタかチェック
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// 採用したアダプタの情報をログに出力(wstring)
			logger_->Log(logger_->GetStream(), logger_->ConvertString(std::format(L"Use Adapter : {}\n", adapterDesc.Description)));
			break;
		}
		useAdapter_ = nullptr; // ソフトウェアアダプタの場合
	}

	// 適切なアダプタが見つからなかったので起動できない
	assert(useAdapter_ != nullptr);
}

void Graphics::CreateD3D12Device() {
	HRESULT hr;

	// 機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };

	// 高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		// 採用したアダプタでデバイスを生成
		hr = D3D12CreateDevice(useAdapter_, featureLevels[i], IID_PPV_ARGS(&device_));
		// 指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr)) {
			// 生成できたのでログ出力を行ってループを抜ける
			logger_->Log(logger_->GetStream(), std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}

	// デバイスの生成がうまくいかなかったので起動できない
	assert(device_ != nullptr);
	// 初期化完了のログを出す
	logger_->Log(logger_->GetStream(), "Complete create D3D12Device.\n");
}

void Graphics::DebugFilter() {
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		// ヤバイエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		// 抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			// Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			// https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		// 指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
	}
}

void Graphics::InitializeFence() {
	HRESULT hr;
	// 初期値0でFenceを作る
	uint64_t fenceValue_ = 0;
	hr = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));

	// FenceのSignalを待つためのイベントを作成する
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);
}

void Graphics::InitializeCommandQueue() {
	HRESULT hr;
	// コマンドキューを生成する
	hr = device_->CreateCommandQueue(&commandQueueDesc_, IID_PPV_ARGS(&commandQueue_));
	// コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));
}

void Graphics::InitializeCommandAllocator() {
	HRESULT hr;
	// コマンドアロケータを生成する
	hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	// コマンドアロケータの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));
}

void Graphics::InitializeCommandList() {
	HRESULT hr;
	// コマンドリストを生成する
	hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
	// コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));
}

void Graphics::InitializeSwapChain(HWND hwnd) {
	HRESULT hr;
	swapChainDesc_.Width = clientWidth_;								// 画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc_.Height = clientHeight_;							// 画面の高さ。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;				// 色の形式
	swapChainDesc_.SampleDesc.Count = 1;							// マルチサンプルしない
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 描画のターゲットとして利用する
	swapChainDesc_.BufferCount = 2;									// ダブルバッファ
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		// モニタにうつしたら、中身を破棄
	// コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), hwnd, &swapChainDesc_, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr));
}

Microsoft::WRL::ComPtr<ID3D12Resource> Graphics::CreateDepthStencilTextureResource(
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
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,					// Heapの設定
		D3D12_HEAP_FLAG_NONE,				// Heapの特殊な設定。特になし。	
		&resourceDesc,						// Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	// 深度値を書き込む状態にしておく
		&depthClearValue,					// Clear最適値
		IID_PPV_ARGS(&resource));			// 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));

	return resource;
}

Model* Graphics::CreateSRV(Model* model) {
	// Textureを読んで転送する
	DirectX::ScratchImage mipImages = LoadTexture(model->GetMaterial());
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = CreateTextureResource(device_, metadata);
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(textureResource, mipImages, device_, commandList_);

	// commandListをCloseし、commandQueue->ExecuteCommandListsを使いキックする
	commandList_->Close();
	Microsoft::WRL::ComPtr<ID3D12CommandList> commandLists[] = { commandList_ };
	commandQueue_->ExecuteCommandLists(1, commandLists->GetAddressOf());
	// 実行を待つ
	fenceValue_++;
	commandQueue_->Signal(fence_.Get(), fenceValue_); // シグナルを送る
	if (fence_->GetCompletedValue() < fenceValue_) {
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		WaitForSingleObject(fenceEvent_, INFINITE); // 待機
	}
	// 実行が完了したので、allocatorとcommandListをResetして次のコマンドを積めるようにする
	commandAllocator_->Reset();
	commandList_->Reset(commandAllocator_.Get(), nullptr);

	// metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	// SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = descriptorHeapManager_->GetSRVHeap()->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = descriptorHeapManager_->GetSRVHeap()->GetGPUDescriptorHandleForHeapStart();
	// 先頭はImGuiが使っているのでその次を使う
	textureSrvHandleCPU.ptr += descriptorHeapManager_->GetSRVHeapSize() * currentSRVIndex_;
	textureSrvHandleGPU.ptr += descriptorHeapManager_->GetSRVHeapSize() * currentSRVIndex_;
	// SRVの生成
	device_->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);
	currentSRVIndex_++; // 次のSRVを使うためにインデックスを進める

	// textureResourceをモデルに設定
	model->SetTextureResource(textureResource);
	// SRVのハンドルをモデルに設定
	model->SetTextureSRVHandle(textureSrvHandleGPU);
	return model;
}

Sprite* Graphics::CreateSRV(Sprite* sprite) {
	// Textureを読んで転送する
	DirectX::ScratchImage mipImages = LoadTexture(sprite->GetMaterial());
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = CreateTextureResource(device_, metadata);
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(textureResource, mipImages, device_, commandList_);

	// commandListをCloseし、commandQueue->ExecuteCommandListsを使いキックする
	commandList_->Close();
	Microsoft::WRL::ComPtr<ID3D12CommandList> commandLists[] = { commandList_ };
	commandQueue_->ExecuteCommandLists(1, commandLists->GetAddressOf());
	// 実行を待つ
	fenceValue_++;
	commandQueue_->Signal(fence_.Get(), fenceValue_); // シグナルを送る
	if (fence_->GetCompletedValue() < fenceValue_) {
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		WaitForSingleObject(fenceEvent_, INFINITE); // 待機
	}
	// 実行が完了したので、allocatorとcommandListをResetして次のコマンドを積めるようにする
	commandAllocator_->Reset();
	commandList_->Reset(commandAllocator_.Get(), nullptr);

	// metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	// SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = descriptorHeapManager_->GetSRVHeap()->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = descriptorHeapManager_->GetSRVHeap()->GetGPUDescriptorHandleForHeapStart();
	// 先頭はImGuiが使っているのでその次を使う
	textureSrvHandleCPU.ptr += descriptorHeapManager_->GetSRVHeapSize() * currentSRVIndex_;
	textureSrvHandleGPU.ptr += descriptorHeapManager_->GetSRVHeapSize() * currentSRVIndex_;
	// SRVの生成
	device_->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);
	currentSRVIndex_++; // 次のSRVを使うためにインデックスを進める

	// textureResourceを設定
	sprite->SetTextureResource(textureResource);
	// SRVのハンドルを設定
	sprite->SetTextureSRVHandle(textureSrvHandleGPU);
	return sprite;
}

DirectX::ScratchImage Graphics::LoadTexture(const std::string& filePath) {
	// テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = logger_->ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	if (FAILED(hr)) {
		return image; // 元画像を返す
	}

	// ミップマップ付きのデータを返す
	return mipImages;
}

Microsoft::WRL::ComPtr<ID3D12Resource> Graphics::CreateTextureResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const DirectX::TexMetadata& metadata) {
	// metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);								// Textureの幅
	resourceDesc.Height = UINT(metadata.height);							// Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);					// mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);				// 奥行き or Textureの配列数
	resourceDesc.Format = metadata.format;									// TextureのFormat
	resourceDesc.SampleDesc.Count = 1;										// サンプル数(1固定)
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);	// Textureの次元数

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;							// VRAM上に作成
	//heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;	// WriteBackポリシーでCPUアクセス可能
	//heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;			// プロセッサの近くに配置

	// Resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,					// Heapの設定
		D3D12_HEAP_FLAG_NONE,				// Heapの特殊な設定。特になし。
		&resourceDesc,						// Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,		// データ転送される設定
		nullptr,							// Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource));			// 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}

Microsoft::WRL::ComPtr<ID3D12Resource> Graphics::UploadTextureData(const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages, const Microsoft::WRL::ComPtr<ID3D12Device>& device, const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList) {
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	// Subresourceの情報を取得
	DirectX::PrepareUpload(device.Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	// サイズの計算
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
	// IntermediateResourceを作成
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(intermediateSize);

	// subresourceのデータを書き込んで転送するコマンドを積む
	UpdateSubresources(commandList.Get(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());

	// Textureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTから
	// D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCEへResourceStateを変更する
	barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier_.Transition.pResource = texture.Get();
	barrier_.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier_);
	return intermediateResource;
}

Microsoft::WRL::ComPtr<ID3D12Resource> Graphics::CreateBufferResource(size_t sizeInBytes) {
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
	HRESULT hr = device_->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));
	return vertexResource;
}

void Graphics::CreateRootSignature() {
	HRESULT hr;
	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;		// PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;						// レジスタ番号0を使う
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	// VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;						// レジスタ番号0を使う

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;	// 0から始まる
	descriptorRange[0].NumDescriptors = 1;		// 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;	// Offsetを自動計算
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;					// DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;								// PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;							// Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);				// Tableで利用する数

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;		// PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1;						// レジスタ番号1を使う

	descriptionRootSignature.pParameters = rootParameters;				// ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);		// 配列の長さ

	// Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // LODの最大値
	staticSamplers[0].ShaderRegister = 0; // レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(hr)) {
		logger_->Log(logger_->GetStream(), reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に生成
	rootSignature_ = nullptr;
	hr = device_->CreateRootSignature(0,
		signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));
}

void Graphics::CreatePipelineState() {
	// ブレンド設定
	// すべての色要素を書き込む
	blendDesc_.RenderTarget[0].BlendEnable = FALSE;
	blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
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
		IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));

	//
	// グリッド用の設定
	//

	gridVSBlob_ = shaderCompiler_->Compile(L"Grid.VS.hlsl",
		L"vs_6_0", logger_);
	assert(gridVSBlob_ != nullptr);

	gridPSBlob_ = shaderCompiler_->Compile(L"Grid.PS.hlsl",
		L"ps_6_0", logger_);
	assert(gridPSBlob_ != nullptr);

	// モデル描画用からコピー
	gridPipelineStateDesc_ = graphicsPipelineStateDesc;

	// トポロジタイプをラインにする
	gridPipelineStateDesc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	// シェーダー適用
	gridPipelineStateDesc_.VS = { gridVSBlob_->GetBufferPointer(), gridVSBlob_->GetBufferSize() };
	gridPipelineStateDesc_.PS = { gridPSBlob_->GetBufferPointer(), gridPSBlob_->GetBufferSize() };

	gridPipelineStateDesc_.InputLayout = gridInputLayoutDesc_;
	gridPipelineStateDesc_.pRootSignature = rootSignature_.Get();

	// PipelineState作成
	hr = device_->CreateGraphicsPipelineState(&gridPipelineStateDesc_,
		IID_PPV_ARGS(&gridPipelineState_));
	assert(SUCCEEDED(hr));
}

void Graphics::SetViewportAndScissor() {
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

void Graphics::CreateLightBuffer() {
	// DirectionalLight用のResource
	directionalLightResource_ = CreateBufferResource(sizeof(DirectionalLight));
	// 書き込むためのアドレスを取得
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	directionalLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightData_->direction = Normalize({ 0.2f, -0.6f, 1.5f });
	directionalLightData_->intensity = 1.0f;
}

void Graphics::InitializeImGui(HWND hwnd) {
	// Imguiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(device_.Get(),
		swapChainDesc_.BufferCount,
		renderTargetManager_->GetRTVDesc_().Format,
		descriptorHeapManager_->GetSRVHeap().Get(),
		descriptorHeapManager_->GetSRVHeap()->GetCPUDescriptorHandleForHeapStart(),
		descriptorHeapManager_->GetSRVHeap()->GetGPUDescriptorHandleForHeapStart()
	);
}

void Graphics::DrawGrid(Camera& camera) {
	// トランスフォーム
	Matrix4x4 worldMatrix = MakeAffineMatrix(gridTransform_);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(camera.viewMatrix_, camera.projectionMatrix_));

	// WVPMatrixを作る
	gridTransformationData_->WVP = worldViewProjectionMatrix;
	gridTransformationData_->World = worldMatrix;

	*gridMaterialData_ = gridMaterial_;
	*gridMaterialDataMark_ = gridMaterialMark_;
	*gridMaterialDataOrigin_ = gridMaterialOrigin_;


	// グリッド用PSOに切り替え
	commandList_->SetPipelineState(gridPipelineState_.Get());
	// トポロジを線に設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	/// 通常線
	// マテリアルCBV
	commandList_->SetGraphicsRootConstantBufferView(0, gridMaterialResource_->GetGPUVirtualAddress());
	// VBV設定
	commandList_->IASetVertexBuffers(0, 1, &gridVBV_);
	// wvp用のCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(1, gridTransformationResource_->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]。
	commandList_->SetGraphicsRootDescriptorTable(2, gridSRVHandleGPU_);
	// ライト
	commandList_->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	// ドローコール
	commandList_->DrawInstanced(UINT(gridVertices_.size()), 1, 0, 0);

	/// 強調線
	// マテリアルCBV
	commandList_->SetGraphicsRootConstantBufferView(0, gridMaterialResourceMark_->GetGPUVirtualAddress());
	// VBV設定
	commandList_->IASetVertexBuffers(0, 1, &gridVBVMark_);
	// wvp用のCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(1, gridTransformationResource_->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]。
	commandList_->SetGraphicsRootDescriptorTable(2, gridSRVHandleGPU_);
	// ライト
	commandList_->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	// ドローコール
	commandList_->DrawInstanced(UINT(gridVerticesMark_.size()), 1, 0, 0);

	/// 原点を通る線
	// マテリアルCBV
	commandList_->SetGraphicsRootConstantBufferView(0, gridMaterialResourceOrigin_->GetGPUVirtualAddress());
	// VBV設定
	commandList_->IASetVertexBuffers(0, 1, &gridVBVOrigin_);
	// wvp用のCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(1, gridTransformationResource_->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]。
	commandList_->SetGraphicsRootDescriptorTable(2, gridSRVHandleGPU_);
	// ライト
	commandList_->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	// ドローコール
	commandList_->DrawInstanced(UINT(gridVerticesOrigin_.size()), 1, 0, 0);
}

void Graphics::InitializeGrid() {

	const int gridHalfWidth = 25; // グリッド数の半分
	const float spacing = 1.0f; // 間隔

	for (int i = -gridHalfWidth; i < gridHalfWidth; ++i) {
		if (i != 0) {
			float x = i * spacing;
			if (abs(i % 10) == 0) {
				// 10mごとの線

				// -z ~ z
				gridVerticesMark_.push_back({ { x,0.0f,-gridHalfWidth * spacing }, {0.0f,0.0f}, {0.0f,1.0f,0.0f} });
				gridVerticesMark_.push_back({ { x,0.0f,gridHalfWidth * spacing }, {0.0f,0.0f}, {0.0f,1.0f,0.0f} });

				// -x ~ x
				gridVerticesMark_.push_back({ { -gridHalfWidth * spacing,0.0f,x}, {0.0f,0.0f}, {0.0f,1.0f,0.0f} });
				gridVerticesMark_.push_back({ {gridHalfWidth * spacing,0.0f,x }, { 0.0f,0.0f }, { 0.0f,1.0f,0.0f } });
			} else {

				// 1mごとの線

				// -z ~ z
				gridVertices_.push_back({ {x,0.0f,-gridHalfWidth * spacing}, {0.0f,0.0f}, {0.0f,1.0f,0.0f} });
				gridVertices_.push_back({ {x,0.0f,gridHalfWidth * spacing }, { 0.0f,0.0f }, { 0.0f,1.0f,0.0f} });

				// -x ~ x
				gridVertices_.push_back({ { -gridHalfWidth * spacing,0.0f,x}, {0.0f,0.0f}, {0.0f,1.0f,0.0f} });
				gridVertices_.push_back({ { gridHalfWidth * spacing,0.0f,x }, {0.0f,0.0f}, {0.0f,1.0f,0.0f} });
			}
		}
	}

	// 原点を通る線
	gridVerticesOrigin_.push_back({ { 0,0,-gridHalfWidth }, {0.0f,0.0f}, {0.0f,1.0f,0.0f} });
	gridVerticesOrigin_.push_back({ { 0,0,gridHalfWidth }, {0.0f,0.0f}, {0.0f,1.0f,0.0f} });
	gridVerticesOrigin_.push_back({ { -gridHalfWidth,0,0 }, {0.0f,0.0f}, {0.0f,1.0f,0.0f} });
	gridVerticesOrigin_.push_back({ { gridHalfWidth,0,0 }, {0.0f,0.0f}, {0.0f,1.0f,0.0f} });

	/// 通常線
	// VertexBuffer作成
	size_t size = sizeof(VertexData) * gridVertices_.size();
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer = CreateBufferResource(size);
	VertexData* dst = nullptr;
	vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&dst));
	memcpy(dst, gridVertices_.data(), size);
	vertexBuffer->Unmap(0, nullptr);

	// VBV作成
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(size);
	vertexBufferView.StrideInBytes = sizeof(VertexData);
	gridVertexBuffer_ = vertexBuffer;
	gridVBV_ = vertexBufferView;

	// マテリアル作成
	gridMaterialResource_ = CreateBufferResource(sizeof(Material));
	gridMaterialResource_->Map(0, nullptr, reinterpret_cast<void**>(&gridMaterialData_));
	gridMaterial_.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	gridMaterial_.useTexture = false;
	gridMaterial_.enableLighting = false;
	gridMaterial_.uvTransform = MakeIdentity4x4();

	/// 強調線
	// VertexBuffer作成
	size = sizeof(VertexData) * gridVerticesMark_.size();
	gridVertexBufferMark_ = CreateBufferResource(size);
	dst = nullptr;
	gridVertexBufferMark_->Map(0, nullptr, reinterpret_cast<void**>(&dst));
	memcpy(dst, gridVerticesMark_.data(), size);
	gridVertexBufferMark_->Unmap(0, nullptr);

	// VBV作成
	gridVBVMark_.BufferLocation = gridVertexBufferMark_->GetGPUVirtualAddress();
	gridVBVMark_.SizeInBytes = UINT(size);
	gridVBVMark_.StrideInBytes = sizeof(VertexData);

	// マテリアル作成
	gridMaterialResourceMark_ = CreateBufferResource(sizeof(Material));
	gridMaterialResourceMark_->Map(0, nullptr, reinterpret_cast<void**>(&gridMaterialDataMark_));
	gridMaterialMark_.color = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
	gridMaterialMark_.useTexture = false;
	gridMaterialMark_.enableLighting = false;
	gridMaterialMark_.uvTransform = MakeIdentity4x4();

	/// 原点の線
	// VertexBuffer作成
	size = sizeof(VertexData) * gridVerticesOrigin_.size();
	gridVertexBufferOrigin_ = CreateBufferResource(size);
	dst = nullptr;
	gridVertexBufferOrigin_->Map(0, nullptr, reinterpret_cast<void**>(&dst));
	memcpy(dst, gridVerticesOrigin_.data(), size);
	gridVertexBufferOrigin_->Unmap(0, nullptr);

	// VBV作成
	gridVBVOrigin_.BufferLocation = gridVertexBufferOrigin_->GetGPUVirtualAddress();
	gridVBVOrigin_.SizeInBytes = UINT(size);
	gridVBVOrigin_.StrideInBytes = sizeof(VertexData);

	// マテリアル作成
	gridMaterialResourceOrigin_ = CreateBufferResource(sizeof(Material));
	gridMaterialResourceOrigin_->Map(0, nullptr, reinterpret_cast<void**>(&gridMaterialDataOrigin_));
	gridMaterialOrigin_.color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	gridMaterialOrigin_.useTexture = false;
	gridMaterialOrigin_.enableLighting = false;
	gridMaterialOrigin_.uvTransform = MakeIdentity4x4();


	// トランスフォーム
	gridTransform_ = { { 1.0f, 1.0f, 1.0f } };
	// リソースを作成
	gridTransformationResource_ = CreateBufferResource(sizeof(TransformationMatrix));

	HRESULT hr = gridTransformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&gridTransformationData_));
	assert(SUCCEEDED(hr));


	// Textureを読んで転送する
	DirectX::ScratchImage mipImages = LoadTexture("Resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	gridTextureResource_ = CreateTextureResource(device_, metadata);
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(gridTextureResource_, mipImages, device_, commandList_);

	// commandListをCloseし、commandQueue->ExecuteCommandListsを使いキックする
	commandList_->Close();
	Microsoft::WRL::ComPtr<ID3D12CommandList> commandLists[] = { commandList_ };
	commandQueue_->ExecuteCommandLists(1, commandLists->GetAddressOf());
	// 実行を待つ
	fenceValue_++;
	commandQueue_->Signal(fence_.Get(), fenceValue_); // シグナルを送る
	if (fence_->GetCompletedValue() < fenceValue_) {
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		WaitForSingleObject(fenceEvent_, INFINITE); // 待機
	}
	// 実行が完了したので、allocatorとcommandListをResetして次のコマンドを積めるようにする
	commandAllocator_->Reset();
	commandList_->Reset(commandAllocator_.Get(), nullptr);

	// metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	// SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = descriptorHeapManager_->GetSRVHeap()->GetCPUDescriptorHandleForHeapStart();
	gridSRVHandleGPU_ = descriptorHeapManager_->GetSRVHeap()->GetGPUDescriptorHandleForHeapStart();
	// 先頭はImGuiが使っているのでその次を使う
	textureSrvHandleCPU.ptr += descriptorHeapManager_->GetSRVHeapSize() * currentSRVIndex_;
	gridSRVHandleGPU_.ptr += descriptorHeapManager_->GetSRVHeapSize() * currentSRVIndex_;
	// SRVの生成
	device_->CreateShaderResourceView(gridTextureResource_.Get(), &srvDesc, textureSrvHandleCPU);
	currentSRVIndex_++; // 次のSRVを使うためにインデックスを進める
}

void Graphics::InitializeSphere() {
	// 分割数
	const uint32_t kSubdivision = 16;
	const float kLonEvery = 2.0f * float(std::numbers::pi) / float(kSubdivision);
	const float kLatEvery = float(std::numbers::pi) / float(kSubdivision);

	// 頂点データの書き込み
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -0.5f * float(std::numbers::pi) + kLatEvery * float(latIndex);
		float latN = lat + kLatEvery;
		// sin,cos
		float cosLat = cos(lat);
		float sinLat = sin(lat);
		float cosLatN = cos(latN);
		float sinLatN = sin(latN);

		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = kLonEvery * float(lonIndex);
			float cosLon = cos(lon);
			float sinLon = sin(lon);
			float cosNextLon = cos(lon + kLonEvery);
			float sinNextLon = sin(lon + kLonEvery);

			// テクスチャ座標
			float u = float(lonIndex) / float(kSubdivision);
			float nextU = float(lonIndex + 1) / float(kSubdivision);
			float v = 1.0f - float(latIndex) / float(kSubdivision);
			float nextV = 1.0f - float(latIndex + 1) / float(kSubdivision);

			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;

			// 頂点位置
			// BL
			sphereDrawer_.vertices.push_back({
			{ cosLat * cosLon,  sinLat,  cosLat * sinLon, 1.0f },		// position
			{ u,  v },													// texcoord
			{Normalize({ cosLat * cosLon,  sinLat,  cosLat * sinLon})}	// normal
				});
			// BR
			sphereDrawer_.vertices.push_back({
				{cosLat * cosNextLon, sinLat,  cosLat * sinNextLon, 1.0f },
				{ nextU, v },
				{Normalize({cosLat * cosNextLon, sinLat,  cosLat * sinNextLon})}
				});
			// TL
			sphereDrawer_.vertices.push_back({
				{cosLatN * cosLon,  sinLatN, cosLatN * sinLon, 1.0f },
				{ u, nextV },
				{Normalize({cosLatN * cosLon,  sinLatN, cosLatN * sinLon})}
				});

			// TR
			sphereDrawer_.vertices.push_back({
				{ cosLatN * cosNextLon, sinLatN, cosLatN * sinNextLon, 1.0f },
				{ nextU, nextV },
				{Normalize({ cosLatN * cosNextLon, sinLatN, cosLatN * sinNextLon})}
				});

			// 同じ座標の頂点を代入
			sphereDrawer_.vertices.push_back(sphereDrawer_.vertices[start + 2]); // TL
			sphereDrawer_.vertices.push_back(sphereDrawer_.vertices[start + 1]); // BR
		}
	}

	// VertexBuffer作成
	size_t size = sizeof(VertexData) * sphereDrawer_.vertices.size();
	sphereDrawer_.vertexBuffer = CreateBufferResource(size);
	VertexData* dst = nullptr;
	sphereDrawer_.vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&dst));
	memcpy(dst, sphereDrawer_.vertices.data(), size);
	sphereDrawer_.vertexBuffer->Unmap(0, nullptr);

	// VBV作成
	sphereDrawer_.vertexBufferView.BufferLocation = sphereDrawer_.vertexBuffer->GetGPUVirtualAddress();
	sphereDrawer_.vertexBufferView.SizeInBytes = UINT(size);
	sphereDrawer_.vertexBufferView.StrideInBytes = sizeof(VertexData);

	// マテリアル作成
	sphereDrawer_.materialResource = CreateBufferResource(sizeof(Material));
	sphereDrawer_.materialResource->Map(0, nullptr, reinterpret_cast<void**>(&sphereDrawer_.materialData));
	sphereDrawer_.material.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	sphereDrawer_.material.useTexture = true;
	sphereDrawer_.material.enableLighting = true;
	sphereDrawer_.material.uvTransform = MakeIdentity4x4();

	// リソースを作成
	sphereDrawer_.transformResource = CreateBufferResource(sizeof(TransformationMatrix));
	HRESULT hr = sphereDrawer_.transformResource->Map(0, nullptr, reinterpret_cast<void**>(&sphereDrawer_.transformData));
	assert(SUCCEEDED(hr));


	// Textureを読んで転送する
	DirectX::ScratchImage mipImages = LoadTexture("Resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	sphereDrawer_.textureResource = CreateTextureResource(device_, metadata);
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(sphereDrawer_.textureResource, mipImages, device_, commandList_);

	// commandListをCloseし、commandQueue->ExecuteCommandListsを使いキックする
	commandList_->Close();
	Microsoft::WRL::ComPtr<ID3D12CommandList> commandLists[] = { commandList_ };
	commandQueue_->ExecuteCommandLists(1, commandLists->GetAddressOf());
	// 実行を待つ
	fenceValue_++;
	commandQueue_->Signal(fence_.Get(), fenceValue_); // シグナルを送る
	if (fence_->GetCompletedValue() < fenceValue_) {
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		WaitForSingleObject(fenceEvent_, INFINITE); // 待機
	}
	// 実行が完了したので、allocatorとcommandListをResetして次のコマンドを積めるようにする
	commandAllocator_->Reset();
	commandList_->Reset(commandAllocator_.Get(), nullptr);

	// metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	// SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSRVHandleCPU = descriptorHeapManager_->GetSRVHeap()->GetCPUDescriptorHandleForHeapStart();
	sphereDrawer_.textureSRVHandleGPU = descriptorHeapManager_->GetSRVHeap()->GetGPUDescriptorHandleForHeapStart();
	// 先頭はImGuiが使っているのでその次を使う
	textureSRVHandleCPU.ptr += descriptorHeapManager_->GetSRVHeapSize() * currentSRVIndex_;
	sphereDrawer_.textureSRVHandleGPU.ptr += descriptorHeapManager_->GetSRVHeapSize() * currentSRVIndex_;
	// SRVの生成
	device_->CreateShaderResourceView(sphereDrawer_.textureResource.Get(), &srvDesc, textureSRVHandleCPU);
	currentSRVIndex_++; // 次のSRVを使うためにインデックスを進める
}

void Graphics::DrawSphere(Transform& transform,Camera& camera) {
	// マテリアル変更を反映
	*sphereDrawer_.materialData = sphereDrawer_.material;

	// トランスフォーム
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(camera.viewMatrix_, camera.projectionMatrix_));

	// WVPMatrixを作る
	sphereDrawer_.transformData->WVP = worldViewProjectionMatrix;
	sphereDrawer_.transformData->World = worldMatrix;

	// マテリアルCBV
	commandList_->SetGraphicsRootConstantBufferView(0, sphereDrawer_.materialResource->GetGPUVirtualAddress());
	// VBV設定
	commandList_->IASetVertexBuffers(0, 1, &sphereDrawer_.vertexBufferView);
	// wvp用のCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(1, sphereDrawer_.transformResource->GetGPUVirtualAddress());
	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]。
	commandList_->SetGraphicsRootDescriptorTable(2, sphereDrawer_.textureSRVHandleGPU);
	// ライト
	commandList_->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	// ドローコール
	commandList_->DrawInstanced(UINT(sphereDrawer_.vertices.size()), 1, 0, 0);
}

D3D12_CPU_DESCRIPTOR_HANDLE Graphics::GetCPUDescriptorHandle(
	const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index) {

	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

void Graphics::ImGuiEditLight() {
	ImGui::ColorEdit3("Color", &directionalLightData_->color.x);
	ImGui::DragFloat3("Direction", &directionalLightData_->direction.x, 0.1f);
	directionalLightData_->direction = Normalize(directionalLightData_->direction);
	ImGui::DragFloat("Intencity", &directionalLightData_->intensity, 0.1f);
	if (ImGui::Button("Reset Light")) {
		directionalLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
		directionalLightData_->direction = Normalize({ 0.2f, -0.6f, 1.5f });
		directionalLightData_->intensity = 1.0f;
	}
}

void Graphics::ImGuiEditSphere() {
	ImGui::DragFloat3("UVTransform.Scale", &sphereSRT_.scale.x,0.01f);
	ImGui::DragFloat3("UVTransform.Rotate", &sphereSRT_.rotate.x,0.01f);
	ImGui::DragFloat3("UVTransform.Translate", &sphereSRT_.translate.x,0.01f);
	sphereDrawer_.material.uvTransform = MakeAffineMatrix(sphereSRT_);
}