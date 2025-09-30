#include "Sprite.h"
#include "Material.h"

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

Sprite* Sprite::Initialize(Graphics* graphics, std::string texturePath, Vector2 size) {
	Sprite* sprite = new Sprite;
	// Sprite用のマテリアルリソースを作る
	sprite->materialResource_ = graphics->CreateBufferResource(sizeof(Material));
	sprite->materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&sprite->materialData_));
	sprite->materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	sprite->materialData_->useTexture = true;
	sprite->materialData_->enableLighting = false;
	sprite->materialData_->uvTransform = MakeIdentity4x4();

	// UVTransform
	Transform uvTransform_{
		{1.0f,1.0f,0.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	// Sprite用のインデックスリソースを作成する
	sprite->indexResource_ = graphics->CreateBufferResource(sizeof(uint32_t) * 6);

	// リソースの先頭のアドレスから使う
	sprite->indexBufferView_.BufferLocation = sprite->indexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズはindex6つ分のサイズ
	sprite->indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_t
	sprite->indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	// インデックスリソースにデータを書き込む
	uint32_t* indexDataSprite = nullptr;
	sprite->indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	indexDataSprite[0] = 0;		indexDataSprite[1] = 1;		indexDataSprite[2] = 2;
	indexDataSprite[3] = 1;		indexDataSprite[4] = 3;		indexDataSprite[5] = 2;

	// Sprite用の頂点リソースを作る
	sprite->vertexResource_ = graphics->CreateBufferResource(sizeof(VertexData) * 4);
	// 頂点バッファビューを作成する
	// リソースの先頭のアドレスから使う
	sprite->vertexBufferView_.BufferLocation = sprite->vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点4つ分のサイズ
	sprite->vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点あたりのサイズ
	sprite->vertexBufferView_.StrideInBytes = sizeof(VertexData);

	// 頂点リソースにデータを書き込む
	sprite->vertexData_ = nullptr;
	// 書き込むためのアドレスを取得
	sprite->vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&sprite->vertexData_));
	// 頂点4つ
	sprite->vertexData_[0].position = { 0.0f,size.y,0.0f,1.0f };	// 左下
	sprite->vertexData_[0].texcoord = { 0.0f,1.0f };
	sprite->vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };		// 左上
	sprite->vertexData_[1].texcoord = { 0.0f,0.0f };
	sprite->vertexData_[2].position = { size.x,size.y,0.0f,1.0f };	// 右下
	sprite->vertexData_[2].texcoord = { 1.0f,1.0f };
	sprite->vertexData_[3].position = { size.x,0.0f,0.0f,1.0f };	// 右上
	sprite->vertexData_[3].texcoord = { 1.0f,0.0f };

	for (UINT i = 0; i < 4; ++i) {
		sprite->vertexData_[i].normal = { 0.0f,0.0f,-1.0f };
	}

	// Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	sprite->transformationResource_ = graphics->CreateBufferResource(sizeof(TransformationMatrix));
	// データを書き込む
	// 書き込むためのアドレスを取得
	sprite->transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&sprite->transformationData_));
	// 単位行列を書き込んでおく
	sprite->transformationData_->WVP = MakeIdentity4x4();
	sprite->transformationData_->World = MakeIdentity4x4();

	// トランスフォーム初期化
	sprite->transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	sprite->texturePath_ = texturePath;

	sprite = graphics->CreateSRV(sprite);

	sprite->size_ = { size.x,size.y,1.0f };

	return sprite;
}

void Sprite::Draw(Graphics& graphics, const Vector4& color) {

	// マテリアルの適用
	material_.color = color;
	*materialData_ = material_;
	graphics.DrawSprite(*this);

	ResetMaterial();
}

void Sprite::ResetMaterial() {
	// マテリアル初期化
	material_.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	material_.useTexture = true;
	material_.enableLighting = true;
	material_.uvTransform = MakeIdentity4x4();
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
	ImGui::ColorEdit3("Color", &materialData_->color.x);
	ImGui::PopID();
}