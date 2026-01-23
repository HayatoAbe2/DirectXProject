#include "InstancedModel.h"
#include "Camera.h"
#include "ResourceManager.h"

void InstancedModel::SetData(std::shared_ptr<ModelData> data, ResourceManager* rm) {
	// データ設定
	data_ = data;

	// 現在のマテリアルを共通マテリアルからコピー
	for (int i = 0; i < data->defaultMaterials_.size(); ++i) {
		auto newMat = std::make_unique<Material>(*data->defaultMaterials_[i].get());
		newMat->Initialize(rm, data->defaultMaterials_[i]->GetData().useTexture, data->defaultMaterials_[i]->GetData().enableLighting);
		newMat->SetTexture(data->defaultMaterials_[i]->GetTexture());


		material_.push_back(std::move(newMat));
	}

	// CBハンドル割り当て(まだID)
	transformCBHandle_ = rm->AllocateTransformCB();
}

void InstancedModel::UpdateInstanceTransform(Camera* camera, const std::vector<Transform>& transforms, std::vector<Vector4>& color) {
	numInstance_ = static_cast<int>(transforms.size());
	// WVPMatrixを作る
	for (int i = 0; i < numInstance_; ++i) {
		Matrix4x4 worldMatrix = MakeAffineMatrix(transforms[i]);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(camera->viewMatrix_, camera->projectionMatrix_));
		instanceTransformationData_[i].WVP = worldViewProjectionMatrix;
		instanceTransformationData_[i].World = worldMatrix;
		instanceTransformationData_[i].WorldInverseTranspose = Transpose(Inverse(worldMatrix));
		instanceTransformationData_[i].Color = color[i];
	}
}

void InstancedModel::UpdateInstanceTransformWithNode(Camera* camera, const Matrix4x4& nodeWorld, const std::vector<Vector4>& colors) {
	for (int i = 0; i < numInstance_; ++i) {
		Matrix4x4 instanceWorld = MakeAffineMatrix(transforms_[i]);
		Matrix4x4 world = instanceWorld * nodeWorld;

		instanceTransformationData_[i].World = world;
		instanceTransformationData_[i].WVP =
			world * camera->viewMatrix_ * camera->projectionMatrix_;
		instanceTransformationData_[i].WorldInverseTranspose =
			Transpose(Inverse(world));

		instanceTransformationData_[i].Color = colors[i];
	}
}
