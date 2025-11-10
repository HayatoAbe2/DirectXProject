#include "InstancedModel.h"
#include "Camera.h"

void InstancedModel::UpdateInstanceTransform(const Camera& camera, const std::vector<Transform> transforms) {
	// WVPMatrixを作る
	for (int i = 0; i < numInstance_; ++i) {
		Matrix4x4 worldMatrix = MakeAffineMatrix(transforms[i]);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(camera.viewMatrix_, camera.projectionMatrix_));
		instanceTransformationData_[i].WVP = worldViewProjectionMatrix;
		instanceTransformationData_[i].World = worldMatrix;
	}
}