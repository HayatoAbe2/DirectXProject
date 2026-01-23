#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>

struct SubMesh {
	std::vector<VertexData> vertices_{};
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	int materialIndex_ = -1;

	std::vector<uint32_t> indices_{};
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer_ = nullptr;
	D3D12_INDEX_BUFFER_VIEW ibv_{};
};
