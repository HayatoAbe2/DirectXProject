#pragma once
#include <dxgi1_6.h>
#include <d3d12.h>
#include <dxcapi.h>
#include <wrl.h>
#include <cstdint>

class DescriptorHeapManager;
class RenderTargetManager {
public:
	/// <summary>
	/// RTV作成
	/// </summary>
	/// <param name="swapChain">スワップチェーン</param>
	/// <param name="device">デバイス</param>
	/// <param name="heapManager">ヒープ管理クラスのインスタンス</param>
	void InitializeSwapChainBuffers(Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain, Microsoft::WRL::ComPtr<ID3D12Device> device, DescriptorHeapManager* heapManager);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);

	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle(size_t index) const { return rtvHandles_[index]; }

	Microsoft::WRL::ComPtr<ID3D12Resource> GetSwapChainResource(size_t index) { return swapChainResources_[index]; }

	D3D12_RENDER_TARGET_VIEW_DESC GetRTVDesc_() { return rtvDesc_; }

private:
	// RTVDesc
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};

	// RTVハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];

	// バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2];
};

