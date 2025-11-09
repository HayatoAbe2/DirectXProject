#pragma once
#include <wrl.h>
#include <cstdint>
#include <d3d12.h>

class DescriptorHeapManager;

class SRVManager {
public:
	// 初期化
	void Initialize(DescriptorHeapManager* descHeapManager,ID3D12Device* device);

	// 描画前
	void PreDraw(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList);

	void SetGraphicsRootDescriptorTable(UINT rootParameterIndex, uint32_t srvIndex, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList);

	// メモリ確保
	uint32_t Allocate();

	// SRV生成
	void CreateTextureSRV(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT format, UINT mipLevels);
	void CreateStructuredBufferSRV(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structuredByteStride);

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t index);


	/// <summary>
	/// DescriptorHeap取得
	/// </summary>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetHeap() { return descriptorHeap_; }

	// サイズ
	uint32_t GetSRVHeapSize() { return descriptorSize_; }

private:
	DescriptorHeapManager* descHeapManager_ = nullptr;
	ID3D12Device* device_ = nullptr;

	// SRVディスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_ = nullptr;
	
	// DescriptorHeapのサイズ
	uint32_t descriptorSize_;

	// 最大数
	const uint32_t kMaxSRVCount_ = 512;

	// 次のindex
	uint32_t useIndex_ = 0;
};

