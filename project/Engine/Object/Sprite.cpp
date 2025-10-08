#include "Sprite.h"
#include "Material.h"

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

void Sprite::Draw(Renderer& renderer) {
	renderer.DrawSprite(*this);
}

void Sprite::UpdateTransform(Camera* camera, float kClientWidth, float kClientHeight, bool useScreenPos) {
	// トランスフォーム
	Matrix4x4 worldMatrix;
	Matrix4x4 worldViewProjectionMatrix;
	if (useScreenPos) {
		worldMatrix = MakeAffineMatrix(transform_);
		Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(kClientWidth), float(kClientHeight), 0.0f, 100.0f);
		worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(camera->viewMatrix_, projectionMatrix));
	} else {
		worldMatrix = MakeAffineMatrix(transform_);
		
		// ピボット補正:中心
		Matrix4x4 matPivot = MakeTranslateMatrix({
			-size_.x / 2.0f,
			-size_.y / 2.0f,
			-size_.z / 2.0f
		});
		worldMatrix = matPivot * worldMatrix;
		worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(camera->viewMatrix_, camera->projectionMatrix_));
	}
	// WVPMatrixを作る
	transformationData_->WVP = worldViewProjectionMatrix;
	transformationData_->World = worldMatrix;
}

void Sprite::ImGuiEdit() {
	ImGui::PushID("Sprite");
	ImGui::DragFloat3("Scale", &transform_.scale.x, 0.01f);
	ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Translate", &transform_.translate.x, 0.01f);
	ImGui::PopID();
}