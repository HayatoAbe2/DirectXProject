#pragma once
#include "Material.h"
#include "VertexData.h"
#include "TransformationMatrix.h"
#include "Texture.h"

#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <string>

class Renderer;
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

	const D3D12_VERTEX_BUFFER_VIEW& GetVBV()const { return vertexBufferView_; }
	const std::vector<VertexData>& GetVertices() const{ return vertices_; }
	const D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSRVHandle() const {return material_->GetTextureSRVHandle();}
	const D3D12_GPU_VIRTUAL_ADDRESS GetCBV()const { return transformationResource_->GetGPUVirtualAddress(); } 
	const std::string& GetMtlPath()const { return mtlFilePath; }
	D3D12_GPU_VIRTUAL_ADDRESS GetMaterialCBV() { return material_->GetCBV()->GetGPUVirtualAddress(); }

	void UpdateMaterial() { material_->UpdateGPU(); }

	/// <summary>
	/// トランスフォーム設定
	/// </summary>
	/// <param name="transform">トランスフォーム</param>
	void SetTransform(const Transform& transform) {
		transform_ = transform;
	}

	// インスタンス描画用
	
	void SetInstance(int num) {
		isInstancing_ = true;
		numInstance_ = num;
	}
	bool IsInstancing() { return isInstancing_; }
	int GetNumInstance() { return numInstance_; }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetInstanceSRVHandle() const { return instanceSRVHandleGPU_; }
	void SetSRVHandle(D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU) { instanceSRVHandleGPU_ = srvHandleGPU; }

	const Microsoft::WRL::ComPtr<ID3D12Resource>& GetInstanceResource() const { return instanceTransformationResource_; }
	void SetInstanceResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource) {
		instanceTransformationResource_ = resource;
	}

	void AddInstanceTransform() {
		if (!isInstancing_) return;
		instanceTransforms_.push_back({});
	}
	void SetInstanceTransforms(int index, const Transform& transform, const Camera& camera) {
		if (!isInstancing_ || index < 0 || index >= numInstance_) return;
		instanceTransforms_[index] = transform;
	}

	void SetInstanceTransformData(TransformationMatrix* data) { instanceTransformationData_ = data; }
	void UpdateInstanceTransform(Model* model, Camera* camera, const Transform* transforms, int numInstance);



	const D3D12_GPU_VIRTUAL_ADDRESS GetInstanceCBV()const { return instanceTransformationResource_->GetGPUVirtualAddress(); }

private:

	// モデルデータ
	std::vector<VertexData> vertices_;
	std::string mtlFilePath;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	Material* material_;
	Transform transform_;

	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_ = nullptr;
	TransformationMatrix* transformationData_ = nullptr;

	// 複数体描画用（必要なときだけ使う）
	bool isInstancing_ = false;
	int numInstance_ = 1;
	D3D12_GPU_DESCRIPTOR_HANDLE instanceSRVHandleGPU_;
	std::vector<Transform> instanceTransforms_;
	Microsoft::WRL::ComPtr<ID3D12Resource> instanceTransformationResource_ = nullptr;
	TransformationMatrix* instanceTransformationData_ = nullptr;
};

