#include "ConstantBufferManager.h"
#include "externals/DirectXTex/d3dx12.h"

ConstantBufferManager::ConstantBufferManager(ID3D12Device* device) {
	// トランスフォームCB初期化
	InitializeTransformCB(device);
}

void ConstantBufferManager::InitializeTransformCB(ID3D12Device* device) {
	// エンティティtransformバッファ初期化
	UINT bufferSize = kCBSize * kMaxObjects;

	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&transformBuffer_)
	);

	// 一度だけMapして保持
	HRESULT hr = transformBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedTransformData_));
	assert(SUCCEEDED(hr));
}

UINT8* ConstantBufferManager::GetTransformPtr(uint32_t transformCBHandle) {
	return mappedTransformData_ + transformCBHandle * kCBSize;
}

D3D12_GPU_VIRTUAL_ADDRESS ConstantBufferManager::GetTransformCBAddress(uint32_t transformCBHandle) {
	return transformBuffer_->GetGPUVirtualAddress() + transformCBHandle * kCBSize;
}