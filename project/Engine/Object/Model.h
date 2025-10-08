#pragma once
#include "Material.h"
#include "VertexData.h"
#include "TransformationMatrix.h"
#include "Texture.h"

#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <string>

class Graphics;
class Camera;
class Model {
public:
	~Model() {
		if (material_) delete material_;
	}

	///
	/// モデル読み込み時のSetter
	///

	/// <summary>
	/// 頂点を設定
	/// </summary>
	/// <param name="vertices">モデルの頂点</param>
	void SetVertices(const std::vector<VertexData>& vertices) { vertices_ = vertices; }

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

	/// <summary>
	/// material設定
	/// </summary>
	/// <param name="materialResource">マテリアル</param>
	void SetMaterial(Material* material) { 
		if (material_) { delete material_; }
		material_ = material;
	}

	/// <summary>
	/// 頂点の追加
	/// </summary>
	/// <param name="vertex">頂点データ</param>
	void AddVertex(const VertexData& vertex) { vertices_.push_back(vertex); }

	/// <summary>
	/// transformリソース設定
	/// </summary>
	/// <param name="resource">リソース</param>
	void SetTransformResource(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource_) { transformationResource_ = resource_; }

	/// <summary>
	/// transformデータ設定
	/// </summary>
	/// <param name="data">データ</param>
	void SetTransformData(TransformationMatrix* data) { transformationData_ = data; }


	/// <summary>
	/// 描画のため、モデルのトランスフォームデータを更新
	/// </summary>
	/// <param name="camera"></param>
	void UpdateTransformation(Camera& camera);

	// 1つのモデルの、複数のトランスフォームを更新する
	void UpdateInstanceTransform(const Transform& transform, const Camera& camera, uint32_t index);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="graphics"></param>
	void Draw(Graphics& graphics, const Vector4& color = {1,1,1,0.3f}, bool useAlphaBlend = 0);

	void EnableInstanceCBV(Graphics& graphics, int maxInstances);

	/// <summary>
	/// 外部CBVをセット(複数のCBVが必要な場合)
	/// </summary>
	/// <param name="address"></param>
	void SetExternalCBV(D3D12_GPU_VIRTUAL_ADDRESS address);

	/// <summary>
	/// 外部CBVを解除
	/// </summary>
	void ClearExternalCBV();

	const D3D12_VERTEX_BUFFER_VIEW& GetVBV()const { return vertexBufferView_; }
	const std::vector<VertexData>& GetVertices() const{ return vertices_; }

	 const D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSRVHandle() const {return material_->GetTextureSRVHandle();}

	const D3D12_GPU_VIRTUAL_ADDRESS GetCBV()const { 
		if (useExternalCBV_) {
			return externalCBVAddress_;
		} else { return transformationResource_->GetGPUVirtualAddress(); } } 
	const std::string& GetMtlPath()const { return mtlFilePath; }
	D3D12_GPU_VIRTUAL_ADDRESS GetMaterialAddress() { return material_->GetCBV()->GetGPUVirtualAddress(); }

	void UpdateMaterial() { material_->UpdateGPU(); }

	/// <summary>
	/// トランスフォーム設定
	/// </summary>
	/// <param name="transform">トランスフォーム</param>
	void SetTransform(const Transform& transform) {
		transform_ = transform;
	}

private:

	// モデルデータ
	std::vector<VertexData> vertices_;
	std::string mtlFilePath;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	Material* material_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Transform transform_;

	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_ = nullptr;
	TransformationMatrix* transformationData_ = nullptr;

	// 複数体描画用（必要なときだけ使う）
	Microsoft::WRL::ComPtr<ID3D12Resource> instanceCBVResource_ = nullptr;
	uint8_t* instanceCBVMappedPtr_ = nullptr;
	UINT instanceCBVStride_ = 0;
	// 外部CBV指定の有無とそのアドレス
	bool useExternalCBV_ = false;
	D3D12_GPU_VIRTUAL_ADDRESS externalCBVAddress_ = 0;
};

