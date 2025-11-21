#include "Sprite.h"
#include "Material.h"

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

void Sprite::UpdateTransform(Vector2 windowSize) {
	Transform transform{};
	transform.scale = { size_.x, size_.y, 1.0f };
	transform.translate = { position_.x,position_.y, 0.0f };
	transform.rotate = { 0.0f,0.0f,rotation_ };
	// トランスフォーム
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform);
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, windowSize.x, windowSize.y, 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, projectionMatrix);
	// WVPMatrixを作る
	transformationData_->WVP = worldViewProjectionMatrix;
	transformationData_->World = worldMatrix;
	//transformationData_->WorldInverseTranspose = Transpose(Inverse(worldMatrix));
}

void Sprite::SetTextureRect(float x, float y, float w, float h) {
	Vector2 texWH = material_->GetTexture()->GetSize();
	
	float left = x / texWH.x;
	float top = y / texWH.y;
	float right = (x + w) / texWH.x;
	float bottom = (y + h) / texWH.y;

	vertexData_[0].texcoord = { left, bottom };
	vertexData_[1].texcoord = { left, top };
	vertexData_[2].texcoord = { right, bottom };
	vertexData_[3].texcoord = { right, top };
}


