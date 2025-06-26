#include "Model.h"
#include "Logger.h"
#include "sstream"
#include "Graphics.h"
#include "Camera.h"
#include "Math.h"

Model* Model::LoadObjFile(const std::string& directoryPath, const std::string& filename,
	Microsoft::WRL::ComPtr<ID3D12Device> device, Graphics &graphics) {
	// 変数の宣言
	Model* model = new Model; // 構築するModeldata
	std::vector<Vector4> positions; // 位置
	std::vector<Vector3> normals; // 法線
	std::vector<Vector2> texcoords; // テクスチャ座標
	std::string line; // ファイルから読んだ1行を格納するもの

	// ファイルを開く
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open()); // 開けなかったらエラー

	// ファイルを読み、ModelDataを構築
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; // 先頭の識別子を読む
		
		// 識別子に応じた処理
		if (identifier == "v") { // 頂点座標
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f; // 右手座標系から左手座標系への変換
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") { // テクスチャ座標
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y; // 左下原点から左上原点への変換
			texcoords.push_back(texcoord);
		} else if (identifier == "vn") { // 法線
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f; // 右手座標系から左手座標系への変換
			normals.push_back(normal);
		} else if (identifier == "f") {
			VertexData triangle[3];
			// 面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				// 頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/'); // /区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				// 要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1]; // 1始まりなので-1
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				triangle[faceVertex] = { position, texcoord, normal };
			}
			// 頂点を逆順で登録することで、周り順を逆にする
			model->vertices_.push_back(triangle[2]);
			model->vertices_.push_back(triangle[1]);
			model->vertices_.push_back(triangle[0]);
		} else if (identifier == "mtllib") {
			// materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			// 基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			model->material_ = LoadMaterialTemplateFile(directoryPath, materialFilename);

			// マテリアルからSRVを作成
			model = graphics.CreateSRV(model);
		}
	}

	// VertexBuffer作成
	size_t size = sizeof(VertexData) * model->vertices_.size();
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer = graphics.CreateBufferResource(device, size);
	VertexData* dst = nullptr;
	vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&dst));
	memcpy(dst, model->vertices_.data(), size);
	vertexBuffer->Unmap(0, nullptr);

	// VBV作成
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(size);
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// Modelに格納
	model->vertexBuffer_ = vertexBuffer;
	model->vertexBufferView_ = vertexBufferView;

	// transformを初期化
	model->transform_ = { { 1.0f, 1.0f, 1.0f } };


	// リソースを作成
	model->transformationResource_ = graphics.CreateBufferResource(device, sizeof(TransformationMatrix));

	HRESULT hr = model->transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&model->transformationData_));
	assert(SUCCEEDED(hr));

	return model;
}

std::string Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	// 変数の宣言
	std::string mtlFilePath; // 構築するMaterialData
	std::string line; // ファイルから読んだ1行を格納するもの
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open()); // 開けなかったらエラー

	// ファイルを読み、MaterialDataを構築
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		// identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			// 連結してファイルパスにする
			mtlFilePath = directoryPath + "/" + textureFilename;
		}
	}
	return mtlFilePath;
}

void Model::UpdateModel(Camera &camera) {
	// モデルのトランスフォーム
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(camera.viewMatrix_, camera.projectionMatrix_));
	// WVPMatrixを作る
	transformationData_->WVP = worldViewProjectionMatrix;
	transformationData_->World = worldMatrix;
}

void Model::Draw(Camera &camera,Graphics &graphics) {
	UpdateModel(camera);
	graphics.DrawModel(*this);
}