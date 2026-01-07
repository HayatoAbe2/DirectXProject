#pragma once
#include "Material.h"
#include "VertexData.h"
#include "InstanceGPUData.h"
#include "Texture.h"
#include "Mesh.h"
#include "Data/ModelData.h"

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
		transforms_.push_back({});
	}
	void SetInstanceTransforms(int index, const Transform& transform) {
		if (index < 0 || index >= numInstance_) return;
		transforms_[index] = transform;
	}
	void SetInstanceTransforms(std::vector<Transform> transforms) {
		transforms.resize(transforms_.size());
		transforms_ = transforms;
	}
	void SetScale(int index, const Vector3& scale) {
		if (index < 0 || index >= numInstance_) return;
		transforms_[index].scale = scale;
	}
	void SetRotate(int index, const Vector3& rotate) {
		if (index < 0 || index >= numInstance_) return;
		transforms_[index].rotate = rotate;
	}
	void SetTranslate(int index, const Vector3& translate) {
		if (index < 0 || index >= numInstance_) return;
		transforms_[index].translate = translate;
	}

	std::vector<Transform> GetTransforms() { return transforms_; }
	void SetInstanceTransformData(InstanceGPUData* data) { instanceTransformationData_ = data; }
	const D3D12_GPU_VIRTUAL_ADDRESS GetInstanceCBV()const { return instanceTransformationResource_->GetGPUVirtualAddress(); }

	void UpdateInstanceTransform(Camera* camera, const std::vector<Transform>& transforms, std::vector<Vector4>& color);

	// データの設定
	void SetData(std::shared_ptr<ModelData> data, ResourceManager* rm);

	// トランスフォームCBハンドルセット
	void SetTransformCBHandle(uint32_t handle) { transformCBHandle_ = handle; }

	// データの取得
	std::shared_ptr<ModelData> GetData() { return data_; }

	// マテリアル
	Material* GetMaterial(int index) { return material_[index].get(); }

	// トランスフォームCBハンドル取得
	uint32_t GetTransformCBHandle() { return transformCBHandle_; }


private:

	// モデルデータ
	std::shared_ptr<ModelData> data_ = nullptr;

	// マテリアル
	std::vector<std::unique_ptr<Material>> material_{};

	// トランスフォームCBハンドル
	uint32_t transformCBHandle_ = 0;

	// 複数体描画用（必要なときだけ使う）
	int numInstance_ = 1;
	D3D12_GPU_DESCRIPTOR_HANDLE instanceSRVHandleGPU_;
	std::vector<Transform> transforms_;
	Microsoft::WRL::ComPtr<ID3D12Resource> instanceTransformationResource_ = nullptr;
	InstanceGPUData* instanceTransformationData_ = nullptr;
};

