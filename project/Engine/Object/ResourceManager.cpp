#include "ResourceManager.h"
#include "Texture.h"
#include "Model.h"
#include "Sprite.h"
#include "InstancedModel.h"
#include "../Io/Logger.h"
#include "../Graphics/CommandListManager.h"
#include "../Graphics/DescriptorHeapManager.h"
#include "../Graphics/SRVManager.h"
#include "../Scene/Camera.h"

#include <string>
#include <sstream>
#include <cassert>
#include <iostream>
#include <format>

ResourceManager::~ResourceManager() {
	// キャッシュしているリソースを解放
	for (auto& texture : textures_) {
		delete texture.second;
	}
	textures_.clear();
	meshes_.clear();
	models_.clear();
}

void ResourceManager::Initialize(const Microsoft::WRL::ComPtr<ID3D12Device>& device, CommandListManager* commandListManager, DescriptorHeapManager* descriptorHeapManager, SRVManager* srvManager, Logger* logger) {
	device_ = device;
	commandListManager_ = commandListManager;
	descriptorHeapManager_ = descriptorHeapManager;
	srvManager_ = srvManager;
	logger_ = logger;
}

DirectX::ScratchImage ResourceManager::LoadTexture(const std::string& filePath) {
	// テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = logger_->ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	if (FAILED(hr)) {
		return image; // 元画像を返す
	}

	// ミップマップ付きのデータを返す
	return mipImages;
}

Microsoft::WRL::ComPtr<ID3D12Resource> ResourceManager::CreateTextureResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const DirectX::TexMetadata& metadata) {
	// metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);								// Textureの幅
	resourceDesc.Height = UINT(metadata.height);							// Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);					// mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);				// 奥行き or Textureの配列数
	resourceDesc.Format = metadata.format;									// TextureのFormat
	resourceDesc.SampleDesc.Count = 1;										// サンプル数(1固定)
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);	// Textureの次元数

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;							// VRAM上に作成
	//heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;	// WriteBackポリシーでCPUアクセス可能
	//heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;			// プロセッサの近くに配置

	// Resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,					// Heapの設定
		D3D12_HEAP_FLAG_NONE,				// Heapの特殊な設定。特になし。
		&resourceDesc,						// Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,		// データ転送される設定
		nullptr,							// Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource_));			// 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource_;
}

Microsoft::WRL::ComPtr<ID3D12Resource> ResourceManager::UploadTextureData(const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages) {
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	// Subresourceの情報を取得
	DirectX::PrepareUpload(device_.Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	// サイズの計算
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
	// IntermediateResourceを作成
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(intermediateSize);

	// subresourceのデータを書き込んで転送するコマンドを積む
	UpdateSubresources(commandListManager_->GetCommandList().Get(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());

	// Textureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTから
	// D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCEへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandListManager_->GetCommandList()->ResourceBarrier(1, &barrier);
	return intermediateResource;
}

Microsoft::WRL::ComPtr<ID3D12Resource> ResourceManager::CreateBufferResource(size_t sizeInBytes) {
	assert(sizeInBytes > 0);

	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;		// uploadHeapを使う
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = sizeInBytes;					// リソースのサイズ
	// バッファの場合はこれらは1にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際にリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	HRESULT hr = device_->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));

	return vertexResource;
}

Texture* ResourceManager::CreateSRV(Texture* texture) {
	if (!texture->GetMtlPath().empty()) {
		// Textureを読んで転送する
		DirectX::ScratchImage mipImages = LoadTexture(texture->GetMtlPath());
		const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
		Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = CreateTextureResource(device_, metadata);
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(textureResource, mipImages);

		// コマンドリストを実行
		commandListManager_->ExecuteAndWait();

		currentSRVIndex_ = srvManager_->Allocate();
		srvManager_->CreateTextureSRV(currentSRVIndex_, textureResource.Get(), metadata.format, UINT(metadata.mipLevels));

		// textureResourceをモデルに設定
		texture->SetResource(textureResource);
		// SRVのハンドルをモデルに設定
		texture->SetSRVHandle(srvManager_->GetGPUHandle(currentSRVIndex_));
	} else {
		texture->SetSRVHandle({ 0 });
		texture->SetResource(nullptr);
	}

	return texture;
}

void ResourceManager::CreateInstancingSRV(InstancedModel* model, const int numInstance_) {
	currentSRVIndex_ = srvManager_->Allocate();
	srvManager_->CreateStructuredBufferSRV(currentSRVIndex_, model->GetInstanceResource().Get(), numInstance_, sizeof(TransformationMatrix));

	// SRVハンドル
	model->SetSRVHandle(srvManager_->GetGPUHandle(currentSRVIndex_));
}

std::shared_ptr<Model> ResourceManager::LoadObjFile(const std::string& directoryPath, const std::string& filename, bool enableLighting) {
	std::shared_ptr<Model> model = std::make_shared<Model>(); // 構築するModel

	// キャッシュにあるか確認
	std::string fullPath = directoryPath + "/" + filename;
	auto it = models_.find(fullPath);
	if (it != models_.end()) {
		return it->second;
	} else {

		// 変数の宣言
		std::vector<Vector4> positions;	// 位置
		std::vector<Vector3> normals;	// 法線
		std::vector<Vector2> texcoords; // テクスチャ座標
		std::vector<VertexData> vertices; // 頂点

		// テクスチャ
		Texture* texture = new Texture;

		// ファイルを開く
		std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
		assert(file.is_open()); // 開けなかったらエラー

		// ファイルを読み、ModelDataを構築
		std::string line; // ファイルから読んだ1行を格納する
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
					uint32_t elementIndices[3]{};
					for (int32_t element = 0; element < 3; ++element) {
						std::string index;
						std::getline(v, index, '/'); // /区切りでインデックスを読んでいく
						if (!index.empty()) {
							elementIndices[element] = std::stoi(index);
						}
					}
					// 要素へのIndexから、実際の要素の値を取得して、頂点を構築する
					Vector4 position = positions[elementIndices[0] - 1]; // 1始まりなので-1
					Vector2 texcoord{};
					if (!texcoords.empty() && elementIndices[1] > 0) {
						texcoord = texcoords[elementIndices[1] - 1];
					}
					Vector3 normal{};
					if (!normals.empty() && elementIndices[2] > 0) {
						normal = normals[elementIndices[2] - 1];
					}
					triangle[faceVertex] = { position, texcoord, normal };
				}
				// 頂点を逆順で登録することで、周り順を逆にする
				vertices.push_back(triangle[2]);
				vertices.push_back(triangle[1]);
				vertices.push_back(triangle[0]);

			} else if (identifier == "mtllib") {
				// materialTemplateLibraryファイルの名前を取得する
				std::string materialFilename;
				s >> materialFilename;

				// 基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
				texture->SetMtlFilePath(LoadMaterialTemplateFile(directoryPath, materialFilename));

				// SRVを作成
				texture = CreateSRV(texture);
			}
		}

		// VertexBuffer作成
		size_t size = sizeof(VertexData) * vertices.size();
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer = CreateBufferResource(size);
		VertexData* dst = nullptr;
		vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&dst));
		memcpy(dst, vertices.data(), size);
		vertexBuffer->Unmap(0, nullptr);

		// VBV作成
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
		vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.SizeInBytes = UINT(size);
		vertexBufferView.StrideInBytes = sizeof(VertexData);

		// Meshに格納
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->SetVertices(vertices);			// 頂点
		mesh->SetVertexBuffer(vertexBuffer);	// 頂点バッファ
		mesh->SetVBV(vertexBufferView);			// 頂点バッファビュー

		// マテリアル初期化
		std::shared_ptr<Material> material = std::make_shared<Material>();
		material->Initialize(this, !texcoords.empty(), enableLighting); // テクスチャ座標情報がなければテクスチャ不使用
		material->SetTexture(texture);	// テクスチャ
		mesh->SetMaterial(material);
		model->AddMeshes(mesh);

		// キャッシュに登録
		models_.insert({ fullPath, model });
	}

	return model;
}

std::shared_ptr<InstancedModel> ResourceManager::LoadObjFile(const std::string& directoryPath, const std::string& filename, const int numInstance) {
	if (numInstance == 0) { assert(false); } // インスタンス数0はないので止める

	// キャッシュにあるか確認
	std::string fullPath = directoryPath + "/" + filename;
	auto it = meshes_.find(fullPath);
	//if (it != meshes_.end()) {
	//	return it->second; // キャッシュにあったのでそれを返す
	//}

	// 変数の宣言
	std::shared_ptr<InstancedModel> model = std::make_shared<InstancedModel>(); // 構築するModel
	std::vector<Vector4> positions;	// 位置
	std::vector<Vector3> normals;	// 法線
	std::vector<Vector2> texcoords; // テクスチャ座標
	std::vector<VertexData> vertices; // 頂点

	// テクスチャ
	Texture* texture = new Texture;

	// ファイルを開く
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open()); // 開けなかったらエラー

	// ファイルを読み、ModelDataを構築
	std::string line; // ファイルから読んだ1行を格納する
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
				uint32_t elementIndices[3]{};
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/'); // /区切りでインデックスを読んでいく
					if (!index.empty()) {
						elementIndices[element] = std::stoi(index);
					}
				}
				// 要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1]; // 1始まりなので-1
				Vector2 texcoord{};
				if (!texcoords.empty() && elementIndices[1] > 0) {
					texcoord = texcoords[elementIndices[1] - 1];
				}
				Vector3 normal{};
				if (!normals.empty() && elementIndices[2] > 0) {
					normal = normals[elementIndices[2] - 1];
				}
				triangle[faceVertex] = { position, texcoord, normal };
			}
			// 頂点を逆順で登録することで、周り順を逆にする
			vertices.push_back(triangle[2]);
			vertices.push_back(triangle[1]);
			vertices.push_back(triangle[0]);

		} else if (identifier == "mtllib") {
			// materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;

			// 基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			texture->SetMtlFilePath(LoadMaterialTemplateFile(directoryPath, materialFilename));

			// SRVを作成
			texture = CreateSRV(texture);
		}
	}

	// VertexBuffer作成
	size_t size = sizeof(VertexData) * vertices.size();
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer = CreateBufferResource(size);
	VertexData* dst = nullptr;
	vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&dst));
	memcpy(dst, vertices.data(), size);
	vertexBuffer->Unmap(0, nullptr);

	// VBV作成
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(size);
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// Meshに格納
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	mesh->SetVertices(vertices);			// 頂点
	mesh->SetVertexBuffer(vertexBuffer);	// 頂点バッファ
	mesh->SetVBV(vertexBufferView);			// 頂点バッファビュー

	// マテリアル初期化
	std::shared_ptr<Material> material = std::make_shared<Material>();
	material->Initialize(this, !texcoords.empty(), true); // テクスチャ座標情報がなければテクスチャ不使用
	material->SetTexture(texture);	// テクスチャ
	mesh->SetMaterial(material);
	model->AddMeshes(mesh);

	model->SetNumInstance(numInstance);
	// インスタンス数分のtransformリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> instanceTransformResource = CreateBufferResource(sizeof(TransformationMatrix) * numInstance);
	TransformationMatrix* transformData = nullptr;
	instanceTransformResource->Map(0, nullptr, reinterpret_cast<void**>(&transformData));
	// 単位行列を書き込んでおく
	for (int i = 0; i < numInstance; ++i) {
		transformData[i].World = MakeIdentity4x4();
		transformData[i].WVP = MakeIdentity4x4();
		transformData[i].WorldInverseTranspose = MakeIdentity4x4();
		model->AddInstanceTransform();
	}
	instanceTransformResource->Unmap(0, nullptr);

	model->SetInstanceResource(instanceTransformResource);
	model->SetInstanceTransformData(transformData);
	CreateInstancingSRV(model.get(), numInstance);


	// キャッシュに登録
	meshes_.insert({ fullPath, mesh });

	return model;
}

std::string ResourceManager::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
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

std::shared_ptr<Sprite> ResourceManager::LoadSprite(std::string texturePath) {
	// キャッシュにあるか確認
	//auto it = sprites_.find(texturePath);
	//if (it != sprites_.end()) {
	//	return it->second; // キャッシュにあったのでそれを返す
	//}


	std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>();

	// UVTransform
	Transform uvTransform_{
		{1.0f,1.0f,0.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	// Sprite用のインデックスリソースを作成する
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource = CreateBufferResource(sizeof(uint32_t) * 6);

	// リソースの先頭のアドレスから使う
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	// 使用するリソースのサイズはindex6つ分のサイズ
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_t
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	// インデックスリソースにデータを書き込む
	uint32_t* indexDataSprite = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	indexDataSprite[0] = 0;		indexDataSprite[1] = 1;		indexDataSprite[2] = 2;
	indexDataSprite[3] = 1;		indexDataSprite[4] = 3;		indexDataSprite[5] = 2;

	sprite->SetIBV(indexBufferView);
	sprite->SetIndexResource(indexResource);

	// Sprite用の頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = CreateBufferResource(sizeof(VertexData) * 4);
	// 頂点バッファビューを作成する
	// リソースの先頭のアドレスから使う
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点4つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	sprite->SetVBV(vertexBufferView);

	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	// 書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	// 頂点4つ
	vertexData[0].position = { 0.0f,1.0f,0.0f,1.0f };	// 左下
	vertexData[0].texcoord = { 0.0f,1.0f };
	vertexData[1].position = { 0.0f,0.0f,0.0f,1.0f };	// 左上
	vertexData[1].texcoord = { 0.0f,0.0f };
	vertexData[2].position = { 1.0f,1.0f,0.0f,1.0f };	// 右下
	vertexData[2].texcoord = { 1.0f,1.0f };
	vertexData[3].position = { 1.0f,0.0f,0.0f,1.0f };	// 右上
	vertexData[3].texcoord = { 1.0f,0.0f };

	for (UINT i = 0; i < 4; ++i) {
		vertexData[i].normal = { 0.0f,0.0f,-1.0f };
	}

	sprite->SetVertexResource(vertexResource);
	sprite->SetVertexData(vertexData);

	// Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	TransformationMatrix* transformationData = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource;
	transformationResource = CreateBufferResource(sizeof(TransformationMatrix));

	// データを書き込む
	// 書き込むためのアドレスを取得
	transformationResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationData));
	// 単位行列を書き込んでおく
	transformationData->WVP = MakeIdentity4x4();
	transformationData->World = MakeIdentity4x4();
	transformationData->WorldInverseTranspose = MakeIdentity4x4();

	sprite->SetTransformData(transformationData);
	sprite->SetTransformResource(transformationResource);

	// TexturePathを設定
	Texture* texture = new Texture;
	texture->SetMtlFilePath(texturePath);
	texture = CreateSRV(texture);

	// Sprite用のマテリアルリソースを作る
	Material* material = new Material;
	material->Initialize(this, true, false); // テクスチャ座標情報がなければテクスチャ不使用
	material->SetTexture(texture);
	sprite->SetMaterial(material);

	// キャッシュに登録
	//sprites_.insert({ texturePath, sprite });

	return sprite;
}
