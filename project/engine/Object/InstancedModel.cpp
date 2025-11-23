#include "InstancedModel.h"
#include "Camera.h"

void InstancedModel::UpdateInstanceTransform(const Camera& camera, const std::vector<Transform> transforms) {
	numInstance_ = static_cast<uint32_t>(transforms.size());
	// WVPMatrixを作る
	for (int i = 0; i < numInstance_; ++i) {
		Matrix4x4 worldMatrix = MakeAffineMatrix(transforms[i]);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(camera.viewMatrix_, camera.projectionMatrix_));
		instanceTransformationData_[i].WVP = worldViewProjectionMatrix;
		instanceTransformationData_[i].World = worldMatrix;
		instanceTransformationData_[i].WorldInverseTranspose = Transpose(Inverse(worldMatrix));
		
	}
}