#include "RenderTargetManager.h"
#include "DescriptorHeapManager.h"
#include <cassert>

void RenderTargetManager::InitializeSwapChainBuffers(IDXGISwapChain4* swapChain, ID3D12Device* device, DescriptorHeapManager* heapManager){
	for (int i = 0; i < 2; ++i) {
		HRESULT hr = swapChain->GetBuffer(i, IID_PPV_ARGS(&swapChainResources_[i]));
		assert(SUCCEEDED(hr));
	}
	
	// RTVの設定
	rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;		// 出力結果をSRGBに変換して書き込む
	rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	// 2dテクスチャとして書き込む
	// ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = GetCPUDescriptorHandle(heapManager->GetRTVHeap().Get(), heapManager->GetRTVHeapSize(), 0);
	// RTVを2つ作るのでディスクリプタ2つ
	rtvHandles_[0] = rtvStartHandle;
	device->CreateRenderTargetView(swapChainResources_[0].Get(), &rtvDesc_, rtvHandles_[0]);
	rtvHandles_[1].ptr = rtvHandles_[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	device->CreateRenderTargetView(swapChainResources_[1].Get(), &rtvDesc_, rtvHandles_[1]);
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetManager::GetCPUDescriptorHandle(
	ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {

	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}