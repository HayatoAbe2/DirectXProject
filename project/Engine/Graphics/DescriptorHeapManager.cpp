#include "DescriptorHeapManager.h"
#include <cassert>

void DescriptorHeapManager::Initialize(ID3D12Device* device) {
		// RTV用のヒープでディスクリプタの数は2。RTVはShader内で触るものではないので、ShaderVisibleはfalse
		rtvDescriptorHeap_ = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
		// DSV用のヒープでディスクリプタの数は1。DSVはShader内で触るものではないので、ShaderVisibleはfalse
		dsvDescriptorHeap_ = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
		// DescriptorSizeを取得
		descriptorSizeRTV_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		descriptorSizeDSV_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DescriptorHeapManager::CreateDescriptorHeap(
	ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	// ディスクリプタヒープが作れなかったので起動できない
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManager::GetCPUDescriptorHandle(
	ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {

	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapManager::GetGPUDescriptorHandle(
	ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {

	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}