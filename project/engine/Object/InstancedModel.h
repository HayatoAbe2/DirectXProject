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
class InstancedModel {
public:
	///
	/// モデル読み込み時のSetter
	///

	/// <summary>
	/// メッシュ追加
	/// </summary>
	/// <param name="mesh">追加するメッシュ</param>
	void AddMeshes(const std::shared_ptr<Mesh>& mesh) { meshes_.push_back(mesh); }

	const std::string& GetMtlPath()const { return mtlFilePath; }

	// メッシュ(全体)の取得
	const std::vector<std::shared_ptr<Mesh>>& GetMeshes() { return meshes_; }

	// インスタンス描画用
	int GetNumInstance() { return numInstance_; }
	void SetNumInstance(int numInstance) { numInstance_ = numInstance; }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetInstanceSRVHandle() const { return instanceSRVHandleGPU_; }
	void SetSRVHandle(D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU) { instanceSRVHandleGPU_ = srvHandleGPU; }

	const Microsoft::WRL::ComPtr<ID3D12Resource>& GetInstanceResource() const { return instanceTransformationResource_; }
	void SetInstanceResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource) {
		instanceTransformationResource_ = resource;
	}

	void AddInstanceTransform() {
		instanceTransforms_.push_back({});
	}
	void SetInstanceTransforms(int index, const Transform& transform, const Camera& camera) {
		if (index < 0 || index >= numInstance_) return;
		instanceTransforms_[index] = transform;
	}

	void SetInstanceTransformData(TransformationMatrix* data) { instanceTransformationData_ = data; }
	const D3D12_GPU_VIRTUAL_ADDRESS GetInstanceCBV()const { return instanceTransformationResource_->GetGPUVirtualAddress(); }

	void UpdateInstanceTransform(const Camera& camera, const std::vector<Transform>& transforms);
private:
	// モデルデータ
	std::string mtlFilePath;

	// メッシュ
	std::vector<std::shared_ptr<Mesh>> meshes_;

	// 複数体描画用（必要なときだけ使う）
	int numInstance_ = 1;
	D3D12_GPU_DESCRIPTOR_HANDLE instanceSRVHandleGPU_;
	std::vector<Transform> instanceTransforms_;
	Microsoft::WRL::ComPtr<ID3D12Resource> instanceTransformationResource_ = nullptr;
	TransformationMatrix* instanceTransformationData_ = nullptr;
};

