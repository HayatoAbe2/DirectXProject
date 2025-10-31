#pragma once
#include "Material.h"
#include "VertexData.h"
#include "TransformationMatrix.h"
#include "Texture.h"
#include "Mesh.h"

#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <string>

class Renderer;
class Camera;
class Model {
public:
	///
	/// モデル読み込み時のSetter
	///

	/// <summary>
	/// メッシュ追加
	/// </summary>
	/// <param name="mesh">追加するメッシュ</param>
	void AddMeshes(const std::shared_ptr<Mesh>& mesh) { meshes_.push_back(mesh); }

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

	const D3D12_GPU_VIRTUAL_ADDRESS GetCBV()const { return transformationResource_->GetGPUVirtualAddress(); } 
	const std::string& GetMtlPath()const { return mtlFilePath; }

	/// <summary>
	/// トランスフォーム設定
	/// </summary>
	/// <param name="transform">トランスフォーム</param>
	void SetTransform(const Transform& transform) {
		transform_ = transform;
	}
	
	// メッシュ(全体)の取得
	const std::vector<std::shared_ptr<Mesh>>& GetMeshes() { return meshes_; }

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
	void UpdateInstanceTransform(Camera* camera, const Transform* transforms);

	const D3D12_GPU_VIRTUAL_ADDRESS GetInstanceCBV()const { return instanceTransformationResource_->GetGPUVirtualAddress(); }

private:
	// モデルデータ
	std::string mtlFilePath;
	Transform transform_;

	// メッシュ
	std::vector<std::shared_ptr<Mesh>> meshes_;

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

