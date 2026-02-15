#pragma once
#include "Graphics/GPUData/TransformationMatrix.h"
#include <cstdint>
#include <wrl.h>
#include <d3d12.h>

class ConstantBufferManager {
public:
	ConstantBufferManager(ID3D12Device* device);
	uint32_t AllocateTransformCB() { return nextId_++; }

	// トランスフォームCBのポインタ
	UINT8* GetTransformPtr(uint32_t transformCBHandle);
	D3D12_GPU_VIRTUAL_ADDRESS GetTransformCBAddress(uint32_t transformCBHandle);

private:
	// トランスフォームバッファ初期化
	void InitializeTransformCB(ID3D12Device* device);

	// transform
	Microsoft::WRL::ComPtr<ID3D12Resource> transformBuffer_;
	UINT8* mappedTransformData_ = nullptr;

	// CBサイズ
	static constexpr UINT kCBSize = (sizeof(TransformationMatrix) + 255) & ~255;
	const UINT kMaxObjects = 4096; // 最大数。もし足りなかったら増やす

	// id(ハンドルにしたい)
	uint32_t nextId_ = 0;
};

