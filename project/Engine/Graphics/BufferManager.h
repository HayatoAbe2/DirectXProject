#pragma once
#include <wrl.h>
#include <d3d12.h>
class BufferManager {
public:
	BufferManager(ID3D12Device* device);
	// バッファリソース作成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateUploadBuffer(size_t sizeInBytes);

private:
	ID3D12Device* device_ = nullptr;
};

