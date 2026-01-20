#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>

struct SubMesh {
	std::vector<VertexData> vertices_{};
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	int materialIndex_ = -1;
};
