#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
class DescriptorHeapManager {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	void Initialize(ID3D12Device* device);

	/// <summary>
	/// DescriptorHeap作成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="heapType"></param>
	/// <param name="numDescriptors"></param>
	/// <param name="shaderVisible"></param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	/// <summary>
	/// ハンドルを計算
	/// </summary>
	/// <param name="descriptorHeap"></param>
	/// <param name="descriptorSize"></param>
	/// <param name="index"></param>
	/// <returns></returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

	/// <summary>
	/// DescriptorHeap取得
	/// </summary>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetRTVHeap() { return rtvDescriptorHeap_; }
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDSVHeap() { return dsvDescriptorHeap_; }

	// サイズ
	uint32_t GetRTVHeapSize() { return descriptorSizeRTV_; }
	uint32_t GetDSVHeapSize() { return descriptorSizeDSV_; }
private:

	// RTV
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;
	// DSV
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_ = nullptr;

	// 各DescriptorHeapのサイズ
	uint32_t descriptorSizeRTV_;
	uint32_t descriptorSizeDSV_;
};

