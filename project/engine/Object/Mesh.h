#pragma once
#include "Material.h"
#include "VertexData.h"
#include "Node.h"
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <memory>
class Mesh {
public:
	/// <summary>
	/// 頂点をまとめて設定
	/// </summary>
	/// <param name="vertices">メッシュの頂点</param>
	void SetVertices(const std::vector<VertexData>& vertices) { vertices_ = vertices; }

	/// <summary>
	/// 頂点の追加
	/// </summary>
	/// <param name="vertex">頂点データ</param>
	void AddVertex(const VertexData& vertex) { vertices_.push_back(vertex); }

	/// <summary>
	/// material設定
	/// </summary>
	/// <param name="materialResource">マテリアル</param>
	void SetMaterial(const std::shared_ptr<Material>& material) {
		material_ = material;
	}

	/// <summary>
	/// 頂点バッファを設定
	/// </summary>
	/// <param name="vb">VertexBuffer</param>
	void SetVertexBuffer(const Microsoft::WRL::ComPtr<ID3D12Resource>& vb) { vertexBuffer_ = vb; }

	/// <summary>
	/// 頂点バッファビューを設定
	/// </summary>
	/// <param name="vbv">vertexBufferView</param>
	void SetVBV(const D3D12_VERTEX_BUFFER_VIEW& vbv) { vertexBufferView_ = vbv; }

	const D3D12_VERTEX_BUFFER_VIEW& GetVBV()const { return vertexBufferView_; }
	const std::vector<VertexData>& GetVertices() const { return vertices_; }
	const D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSRVHandle() const { return material_->GetTextureSRVHandle(); }
	D3D12_GPU_VIRTUAL_ADDRESS GetMaterialCBV() { return material_->GetCBV()->GetGPUVirtualAddress(); }
	void UpdateMaterial() { material_->UpdateGPU(); }
	Material* GetMaterial() { return material_.get(); }


	// ノード
	Node rootNode;
private:
	// 頂点
	std::vector<VertexData> vertices_;
	// マテリアル
	std::shared_ptr<Material> material_;

	// VB
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_ = nullptr;
	// VBV
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
};

