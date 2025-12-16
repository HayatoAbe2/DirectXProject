#pragma once
#include "Data/ModelData.h"

#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <string>

class Renderer;
class Camera;
class ResourceManager;
class Model {
public:

	///
	/// Setter
	///

	void SetTransform(const Transform& transform) { transform_ = transform; }
	void SetScale(const Vector3& scale) { transform_.scale = scale; }
	void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { transform_.translate = translate; }

	// データの設定
	void CopyModelData(std::shared_ptr<ModelData> data, ResourceManager* rm);

	// トランスフォームCBハンドルセット
	void SetTransformCBHandle(uint32_t handle) { transformCBHandle_ = handle; }

	///
	/// Getter
	///

	// トランスフォーム
	Transform GetTransform() { return transform_; }

	// データの取得
	std::shared_ptr<const ModelData> GetData() { return data_; }

	// マテリアル取得
	Material* GetMaterial(int index) { return material_[index].get(); }

	// トランスフォームCBハンドル取得
	uint32_t GetTransformCBHandle() { return transformCBHandle_; }

private:
	// トランスフォーム
	Transform transform_ = { { 1,1,1 },{}, {} };

	// モデルデータ
	std::shared_ptr<ModelData> data_ = nullptr;

	// マテリアル
	std::vector<std::unique_ptr<Material>> material_{};

	// トランスフォームCBハンドル
	uint32_t transformCBHandle_ = 0;
};

