#include "SRVManager.h"
#include "DescriptorHeapManager.h"
#include <cassert>

void SRVManager::Initialize(DescriptorHeapManager* descHeapManager, ID3D12Device* device) {
	descHeapManager_ = descHeapManager;
	device_ = device;

	// SRV用ヒープ
	descriptorHeap_ = descHeapManager->CreateDescriptorHeap(device_, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxSRVCount_, true);

	// Descriptor1個のサイズ
	descriptorSize_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void SRVManager::PreDraw(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList) {
	// 描画用DescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap_.Get() };
	commandList->SetDescriptorHeaps(1, descriptorHeaps);	
}

void SRVManager::SetGraphicsRootDescriptorTable(UINT rootParameterIndex, uint32_t srvIndex, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList) {
	commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, descHeapManager_->GetGPUDescriptorHandle(descriptorHeap_.Get(),descriptorSize_, srvIndex));
}

uint32_t SRVManager::Allocate() {
	assert(useIndex_ < kMaxSRVCount_);

	// 現在インデックスを返す
	int index = useIndex_;
	useIndex_++;
	return index;
}

void SRVManager::CreateTextureSRV(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT format, UINT mipLevels) {
	// metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = mipLevels;

	// SRVの生成
	device_->CreateShaderResourceView(pResource, &srvDesc, descHeapManager_->GetCPUDescriptorHandle(descriptorHeap_.Get(),descriptorSize_,srvIndex));
}

void SRVManager::CreateStructuredBufferSRV(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structuredByteStride) {
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = numElements;
	instancingSrvDesc.Buffer.StructureByteStride = structuredByteStride;

	// SRVの生成
	device_->CreateShaderResourceView(pResource, &instancingSrvDesc, descHeapManager_->GetCPUDescriptorHandle(descriptorHeap_.Get(),descriptorSize_,srvIndex));
}

D3D12_CPU_DESCRIPTOR_HANDLE SRVManager::GetCPUHandle(uint32_t index) {
	return descHeapManager_->GetCPUDescriptorHandle(descriptorHeap_.Get(), descriptorSize_, index);
}

D3D12_GPU_DESCRIPTOR_HANDLE SRVManager::GetGPUHandle(uint32_t index) {
	return descHeapManager_->GetGPUDescriptorHandle(descriptorHeap_.Get(), descriptorSize_, index);
}