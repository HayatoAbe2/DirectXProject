#include "Sprite.h"

void Sprite::Initialize(ID3D12Device* device, Graphics* graphics,std::string texturePath) {
	// Sprite用のマテリアルリソースを作る
	materialResource_ = graphics->CreateBufferResource(device, sizeof(Material));
	Material* materialData_ = nullptr;
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->useTexture = true;
	materialData_->enableLighting = false;
	materialData_->uvTransform = MakeIdentity4x4();

	// UVTransform
	Transform uvTransform_{
		{1.0f,1.0f,0.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	// Sprite用のインデックスリソースを作成する
	indexResource_ = graphics->CreateBufferResource(device, sizeof(uint32_t) * 6);
	
	// リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズはindex6つ分のサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_t
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	// インデックスリソースにデータを書き込む
	uint32_t* indexDataSprite = nullptr;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	indexDataSprite[0] = 0;		indexDataSprite[1] = 1;		indexDataSprite[2] = 2;
	indexDataSprite[3] = 1;		indexDataSprite[4] = 3;		indexDataSprite[5] = 2;

	// Sprite用の頂点リソースを作る
	vertexResource_ = graphics->CreateBufferResource(device, sizeof(VertexData) * 4);
	// 頂点バッファビューを作成する
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点4つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	// 頂点リソースにデータを書き込む
	vertexData_ = nullptr;
	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	// 頂点4つ
	vertexData_[0].position = { 0.0f,360.0f,0.0f,1.0f };	// 左下
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };		// 左上
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[2].position = { 640.0f,360.0f,0.0f,1.0f };	// 右下
	vertexData_[2].texcoord = { 1.0f,1.0f };
	vertexData_[3].position = { 640.0f,0.0f,0.0f,1.0f };	// 右上
	vertexData_[3].texcoord = { 1.0f,0.0f };

	for (UINT i = 0; i < 4; ++i) {
		vertexData_[i].normal = { 0.0f,0.0f,-1.0f };
	}

	// Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	transformationMatrixResource_ = graphics->CreateBufferResource(device, sizeof(TransformationMatrix));
	// データを書き込む
	// 書き込むためのアドレスを取得
	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
	// 単位行列を書き込んでおく
	transformationMatrixData_->WVP = MakeIdentity4x4();
	transformationMatrixData_->World = MakeIdentity4x4();

	// トランスフォーム初期化
	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	texturePath_ = texturePath;
}

void Sprite::SetPosition(const Vector3& pos) {
    transform_.translate = pos;
}

void Sprite::SetScale(const Vector3& scale) {
    transform_.scale = scale;
}

void Sprite::SetRotationZ(float angleRadians) {
    transform_.rotate.z = angleRadians;
}
