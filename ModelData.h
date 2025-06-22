#pragma once
#include "Material.h"
#include <vector>
#include "VertexData.h"
#include <d3d12.h>

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
};