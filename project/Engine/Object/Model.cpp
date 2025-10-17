#include "Model.h"
#include "../Io/Logger.h"
#include "../Graphics/Renderer.h"
#include "../Scene/Camera.h"
#include "../Math/MathUtils.h"

#include <sstream>
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

void Model::UpdateTransformation(Camera& camera) {
	// モデルのトランスフォーム
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(camera.viewMatrix_, camera.projectionMatrix_));
	// WVPMatrixを作る
	transformationData_->WVP = worldViewProjectionMatrix;
	transformationData_->World = worldMatrix;
}

void Model::UpdateInstanceTransform(Model* model, Camera* camera, const Transform* transforms, int numInstance) {
	assert(model);

	// WVPMatrixを作る
	for (int i = 0; i < numInstance; ++i) {
		Matrix4x4 worldMatrix = MakeAffineMatrix(transforms[i]);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(camera->viewMatrix_, camera->projectionMatrix_));
		instanceTransformationData_[i].WVP = worldViewProjectionMatrix;
		instanceTransformationData_[i].World = worldMatrix;
	}
}