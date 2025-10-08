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

void Model::EnableInstanceCBV(Renderer& renderer, int maxInstances) {
	if (maxInstances <= 0) { return; }
	const UINT kCBSize = (sizeof(TransformationMatrix) + 255) & ~255;
	instanceCBVStride_ = kCBSize;

	instanceCBVResource_ = renderer.CreateBufferResource(kCBSize * maxInstances);
	instanceCBVResource_->Map(0, nullptr, reinterpret_cast<void**>(&instanceCBVMappedPtr_));
}

void Model::SetExternalCBV(D3D12_GPU_VIRTUAL_ADDRESS address) {
	externalCBVAddress_ = address;
	useExternalCBV_ = true;
}
void Model::ClearExternalCBV() {
	useExternalCBV_ = false;
	externalCBVAddress_ = 0;
}

void Model::UpdateInstanceTransform(const Transform& transform, const Camera& camera, uint32_t index) {
	// 行列計算
	Matrix4x4 world = MakeAffineMatrix(transform);
	Matrix4x4 wvp = Multiply(world, Multiply(camera.viewMatrix_, camera.projectionMatrix_));

	// データ準備
	TransformationMatrix data = { wvp, world };

	// インスタンスCBVへ書き込み
	assert(instanceCBVMappedPtr_ != nullptr);
	std::memcpy(
		instanceCBVMappedPtr_ + index * instanceCBVStride_,
		&data,
		sizeof(data)
	);

	// 外部CBVの指定
	SetExternalCBV(instanceCBVResource_->GetGPUVirtualAddress() + index * instanceCBVStride_);
}