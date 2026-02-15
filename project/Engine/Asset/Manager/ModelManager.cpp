#include "ModelManager.h"
#include "Asset/Manager/TextureManager.h"
#include "Asset/Model/Model.h"
#include "Asset/Model/InstancedModel.h"
#include "Asset/Model/ModelData.h"

#include "Graphics/DirectXContext.h"
#include "Io/Logger.h"
#include "Graphics/GPUData/VertexData.h"
#include "Graphics/CommandListManager.h"
#include "Graphics/DescriptorHeapManager.h"
#include "Graphics/SRVManager.h"
#include "Graphics/BufferManager.h"
#include "Graphics/ConstantBufferManager.h"

#include <cassert>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Graphics/GPUData/InstanceGPUData.h>

ModelManager::ModelManager(DirectXContext* dxContext, Logger* logger, TextureManager* textureManager) {
	// デバイス
	device_ = dxContext->GetDeviceManager()->GetDevice().Get();
	// CommandList管理クラス
	commandListManager_ = dxContext->GetCommandListManager();
	// DescriptorHeap管理クラス
	descriptorHeapManager_ = dxContext->GetDescriptorHeapManager();
	// SRV管理クラス
	srvManager_ = dxContext->GetSRVManager();
	// バッファ管理クラス
	bufferManager_ = dxContext->GetBufferManager();
	// CB管理クラス
	cbManager_ = dxContext->GetConstantBufferManager();
	// ログ出力クラス
	logger_ = logger;
	// テクスチャ管理クラス
	textureManager_ = textureManager;
}

std::unique_ptr<Model> ModelManager::Load(const std::string& directoryPath, const std::string& filename, bool enableLighting) {
	std::unique_ptr<Model> model = std::make_unique<Model>(); // 構築するModel
	std::shared_ptr<ModelData> modelData = std::make_shared<ModelData>(); // データ

	// トランスフォームCB割り当て
	model->SetTransformCBHandle(cbManager_->AllocateTransformCB());

	// assimpでモデル作成
	Assimp::Importer importer;
	// ファイルパス
	std::string filePath = directoryPath + "/" + filename;
	// obj->DirectX12変換
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate);

	if (!scene) {
		std::string error = importer.GetErrorString();
		assert(false && "Assimp failed to load model");
		return nullptr;
	}

	// rootNode
	model->SetRootNode(std::move(ReadNode(scene->mRootNode)));

	// キャッシュにあるか確認
	std::string fullPath = directoryPath + "/" + filename;
	auto it = modelDataCache_.find(fullPath);
	if (it != modelDataCache_.end()) {
		// あった場合、マテリアルを作成しなおす
		auto cacheData = it->second;
		model->CopyModelData(cacheData, bufferManager_);

	} else {
		// メッシュ数設定
		modelData->meshes.resize(scene->mNumMeshes);

		for (uint32_t aiMeshIndex = 0; aiMeshIndex < scene->mNumMeshes; ++aiMeshIndex) {
			aiMesh* aiMesh = scene->mMeshes[aiMeshIndex];

			// subMesh生成
			SubMesh primitive = CreateSubMesh(aiMesh);

			// Mesh がまだ無ければ作る
			if (!modelData->meshes[aiMeshIndex]) {
				modelData->meshes[aiMeshIndex] = std::make_unique<Mesh>();
			}

			// メッシュにsubMeshを追加
			modelData->meshes[aiMeshIndex]->GetPrimitives().push_back(primitive);
		}

		///
		/// マテリアルの設定
		/// 

		for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
			auto texture = std::make_shared<Texture>();

			aiMaterial* aiMaterial = scene->mMaterials[materialIndex];
			if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
				aiString textureFilePath;
				aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);

				texture->SetMtlFilePath(directoryPath + "/" + textureFilePath.C_Str());
			} else {
				texture->SetMtlFilePath("Resources/white1x1.png");
			}

			// SRVを作成
			textureManager_->CreateTextureSRV(texture);

			// マテリアル初期化
			std::unique_ptr<Material> material = std::make_unique<Material>();
			bool useTexture = !texture->GetMtlPath().empty();
			material->Initialize(bufferManager_, useTexture, enableLighting); // テクスチャ座標情報がなければテクスチャ不使用
			material->SetTexture(texture);	// テクスチャ

			aiColor4D baseColor(1, 1, 1, 1);
			aiGetMaterialColor(aiMaterial, AI_MATKEY_BASE_COLOR, &baseColor);

			auto data = material->GetData();
			data.color = {
				baseColor.r,
				baseColor.g,
				baseColor.b,
				baseColor.a
			};
			material->SetData(data);

			// モデルデータ作成
			modelData->defaultMaterials_.push_back(std::move(material));
		}

		// モデルにデータの参照
		model->CopyModelData(modelData, bufferManager_);

		// キャッシュに登録
		modelDataCache_.insert({ fullPath, modelData });
	}

	return model;
}

std::unique_ptr<InstancedModel> ModelManager::Load(const std::string& directoryPath, const std::string& filename, const int numInstance, bool enableLighting) {
	if (numInstance == 0) { assert(false); } // インスタンス数0の場合止める

	std::unique_ptr<InstancedModel> model = std::make_unique<InstancedModel>(); // 構築するModel
	std::shared_ptr<ModelData> modelData = std::make_shared<ModelData>(); // データ

	model->SetTransformCBHandle(cbManager_->AllocateTransformCB());

	// assimpでモデル作成
	Assimp::Importer importer;
	// ファイルパス
	std::string filePath = directoryPath + "/" + filename;
	// obj->DirectX12変換
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate);

	// rootNode
	model->SetRootNode(ReadNode(scene->mRootNode));

	// キャッシュにあるか確認
	std::string fullPath = directoryPath + "/" + filename;
	auto it = modelDataCache_.find(fullPath);
	if (it != modelDataCache_.end()) {

		// あった場合、マテリアルを作成しなおす
		auto cacheData = it->second;
		model->CopyModelData(cacheData, bufferManager_, cbManager_);

		model->SetNumInstance(numInstance);
		// インスタンス数分のtransformリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> instanceTransformResource = bufferManager_->CreateUploadBuffer(sizeof(InstanceGPUData) * numInstance);
		InstanceGPUData* transformData = nullptr;
		instanceTransformResource->Map(0, nullptr, reinterpret_cast<void**>(&transformData));
		// 単位行列を書き込んでおく
		for (int i = 0; i < numInstance; ++i) {
			transformData[i].World = MakeIdentity4x4();
			transformData[i].WVP = MakeIdentity4x4();
			transformData[i].WorldInverseTranspose = MakeIdentity4x4();
			model->AddInstanceTransform();
		}

		model->SetInstanceResource(instanceTransformResource);
		model->SetInstanceTransformData(transformData);
		CreateInstancingSRV(model.get(), numInstance);
		return model; // キャッシュにあったのでそれを返す
	}

	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		// 新規メッシュ
		std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();

		aiMesh* aiMesh = scene->mMeshes[meshIndex];
		assert(aiMesh->HasNormals());
		assert(aiMesh->HasTextureCoords(0));

		// subMesh
		SubMesh subMesh;
		subMesh.materialIndex_ = aiMesh->mMaterialIndex;

		for (uint32_t faceIndex = 0; faceIndex < aiMesh->mNumFaces; ++faceIndex) {
			aiFace& face = aiMesh->mFaces[faceIndex];

			assert(face.mNumIndices == 3); // 三角形
			for (uint32_t i = 0; i < face.mNumIndices; ++i) {

				///
				/// メッシュ頂点の設定
				/// 

				uint32_t vertexIndex = face.mIndices[i];
				aiVector3D& position = aiMesh->mVertices[vertexIndex];
				aiVector3D& normal = aiMesh->mNormals[vertexIndex];
				aiVector3D& texcoord = aiMesh->mTextureCoords[0][vertexIndex];

				// 頂点データ
				VertexData vertex;
				vertex.position = { position.x,position.y,position.z, 1.0f };
				vertex.normal = { normal.x,normal.y,normal.z };
				vertex.texcoord = { texcoord.x, texcoord.y };
				vertex.position.x *= -1.0f; // 左手系
				vertex.normal.x *= -1.0f;
				if (aiMesh->HasVertexColors(0)) {
					// 頂点カラー
					aiColor4D& color = aiMesh->mColors[0][vertexIndex];
					vertex.color = { color.r, color.g, color.b, color.a };
				} else {
					// なかったら白色
					vertex.color = { 1.0f,1.0f,1.0f,1.0f };
				}

				subMesh.vertices_.push_back(vertex);
			}
		}

		// VertexBuffers
		size_t size = sizeof(VertexData) * subMesh.vertices_.size();
		subMesh.vertexBuffer_ = bufferManager_->CreateUploadBuffer(size);
		VertexData* dst = nullptr;
		subMesh.vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&dst));
		memcpy(dst, subMesh.vertices_.data(), size);
		subMesh.vertexBuffer_->Unmap(0, nullptr);

		// VBV
		subMesh.vertexBufferView_.BufferLocation = subMesh.vertexBuffer_->GetGPUVirtualAddress();
		subMesh.vertexBufferView_.SizeInBytes = UINT(size);
		subMesh.vertexBufferView_.StrideInBytes = sizeof(VertexData);

		// メッシュに追加
		mesh->GetPrimitives().push_back(subMesh);

		// モデルデータにメッシュ追加
		modelData->meshes.push_back(std::move(mesh));
	}

	// メッシュ
	modelData->meshes.resize(scene->mNumMeshes);

	for (uint32_t aiMeshIndex = 0; aiMeshIndex < scene->mNumMeshes; ++aiMeshIndex) {
		aiMesh* aiMesh = scene->mMeshes[aiMeshIndex];

		// subMesh生成
		SubMesh primitive = CreateSubMesh(aiMesh);

		// Mesh がまだ無ければ作る
		if (!modelData->meshes[aiMeshIndex]) {
			modelData->meshes[aiMeshIndex] = std::make_unique<Mesh>();
		}

		// メッシュにsubMeshを追加
		modelData->meshes[aiMeshIndex]->GetPrimitives().push_back(primitive);
	}

	///
	/// マテリアルの設定
	/// 

	std::shared_ptr<Texture> texture = std::make_shared<Texture>();	// テクスチャ

	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* aiMaterial = scene->mMaterials[materialIndex];
		if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);

			// テクスチャ設定
			texture->SetMtlFilePath(directoryPath + "/" + textureFilePath.C_Str());
		} else {
			texture->SetMtlFilePath("Resources/white1x1.png");
		}

		// SRVを作成
		textureManager_->CreateTextureSRV(texture);

		// マテリアル初期化
		std::unique_ptr<Material> material = std::make_unique<Material>();
		bool useTexture = false;
		if (texture->GetMtlPath() != "") { useTexture = true; }
		material->Initialize(bufferManager_, useTexture, enableLighting); // テクスチャ座標情報がなければテクスチャ不使用
		material->SetTexture(texture);	// テクスチャ

		aiColor4D baseColor(1, 1, 1, 1);
		aiGetMaterialColor(aiMaterial, AI_MATKEY_BASE_COLOR, &baseColor);

		auto data = material->GetData();
		data.color = {
			baseColor.r,
			baseColor.g,
			baseColor.b,
			baseColor.a
		};
		material->SetData(data);

		// モデルデータ作成
		modelData->defaultMaterials_.push_back(std::move(material));
	}

	// モデルにデータの参照
	model->CopyModelData(modelData, bufferManager_, cbManager_);

	// キャッシュに登録
	modelDataCache_.insert({ fullPath, std::move(modelData) });

	///
	/// インスタンシング用の設定
	///

	model->SetNumInstance(numInstance);
	// インスタンス数分のtransformリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> instanceTransformResource = bufferManager_->CreateUploadBuffer(sizeof(InstanceGPUData) * numInstance);
	InstanceGPUData* transformData = nullptr;
	instanceTransformResource->Map(0, nullptr, reinterpret_cast<void**>(&transformData));
	// 単位行列を書き込んでおく
	for (int i = 0; i < numInstance; ++i) {
		transformData[i].World = MakeIdentity4x4();
		transformData[i].WVP = MakeIdentity4x4();
		transformData[i].WorldInverseTranspose = MakeIdentity4x4();
		model->AddInstanceTransform();
	}

	model->SetInstanceResource(instanceTransformResource);
	model->SetInstanceTransformData(transformData);
	CreateInstancingSRV(model.get(), numInstance);

	return model;
}

SubMesh ModelManager::CreateSubMesh(aiMesh* aiMesh) {
	SubMesh subMesh{};
	subMesh.materialIndex_ = aiMesh->mMaterialIndex;

	///
	/// index
	///
	for (uint32_t faceIndex = 0; faceIndex < aiMesh->mNumFaces; ++faceIndex) {
		aiFace& face = aiMesh->mFaces[faceIndex];

		assert(face.mNumIndices == 3); // 三角形
		for (uint32_t i = 0; i < face.mNumIndices; ++i) {
			subMesh.indices_.push_back(face.mIndices[i]);
		}
	}

	///
	/// メッシュ頂点の設定
	/// 

	subMesh.vertices_.resize(aiMesh->mNumVertices);
	for (uint32_t v = 0; v < aiMesh->mNumVertices; ++v) {
		aiVector3D& position = aiMesh->mVertices[v];
		// 頂点データ
		VertexData vertex;
		vertex.position = { position.x,position.y,position.z, 1.0f };
		vertex.position.x *= -1.0f; // 左手系

		// 法線
		if (aiMesh->HasNormals()) {
			aiVector3D& normal = aiMesh->mNormals[v];
			vertex.normal = { normal.x,normal.y,normal.z };
			vertex.normal.x *= -1.0f;
		} else {
			// 法線がない場合
			vertex.normal = { 0,1,0 };
		}

		// TexCoord
		if (aiMesh->HasTextureCoords(0)) {
			aiVector3D& texcoord = aiMesh->mTextureCoords[0][v];
			vertex.texcoord = { texcoord.x, texcoord.y };
		} else {
			vertex.texcoord = {};
		}

		if (aiMesh->HasVertexColors(0)) {
			// 頂点カラー
			aiColor4D& color = aiMesh->mColors[0][v];
			vertex.color = { color.r, color.g, color.b, color.a };
		} else {
			// なかったら白色
			vertex.color = { 1.0f,1.0f,1.0f,1.0f };
		}

		subMesh.vertices_[v] = vertex;
	}

	// VertexBuffers
	size_t size = sizeof(VertexData) * subMesh.vertices_.size();
	subMesh.vertexBuffer_ = bufferManager_->CreateUploadBuffer(size);
	VertexData* dst = nullptr;
	subMesh.vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&dst));
	memcpy(dst, subMesh.vertices_.data(), size);
	subMesh.vertexBuffer_->Unmap(0, nullptr);

	// VBV
	subMesh.vertexBufferView_.BufferLocation = subMesh.vertexBuffer_->GetGPUVirtualAddress();
	subMesh.vertexBufferView_.SizeInBytes = UINT(size);
	subMesh.vertexBufferView_.StrideInBytes = sizeof(VertexData);

	// IBV
	size_t indexSize = sizeof(uint32_t) * subMesh.indices_.size();
	subMesh.indexBuffer_ = bufferManager_->CreateUploadBuffer(size);

	uint32_t* dst2 = nullptr;
	subMesh.indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&dst2));
	memcpy(dst2, subMesh.indices_.data(), indexSize);
	subMesh.indexBuffer_->Unmap(0, nullptr);

	subMesh.ibv_.BufferLocation = subMesh.indexBuffer_->GetGPUVirtualAddress();
	subMesh.ibv_.SizeInBytes = UINT(indexSize);
	subMesh.ibv_.Format = DXGI_FORMAT_R32_UINT;

	return subMesh;
}

void ModelManager::CreateInstancingSRV(InstancedModel* model, const int numInstance_) {
	auto index = srvManager_->Allocate();
	srvManager_->CreateStructuredBufferSRV(index, model->GetInstanceResource().Get(), numInstance_, sizeof(InstanceGPUData));

	// SRVハンドル
	model->SetSRVHandle(srvManager_->GetGPUHandle(index));
}

std::unique_ptr<ModelNode> ModelManager::ReadNode(aiNode* aiNode) {
	std::unique_ptr<ModelNode> node = std::make_unique<ModelNode>();
	aiMatrix4x4 aiLocalMatrix = aiNode->mTransformation;
	aiLocalMatrix.Transpose(); // 行ベクトルにする
	node->localMatrix = ConvertAssimpMatrixToLHRow(aiLocalMatrix);

	// meshのindex
	node->meshIndices.reserve(aiNode->mNumMeshes);
	for (uint32_t i = 0; i < aiNode->mNumMeshes; ++i) {
		node->meshIndices.push_back(aiNode->mMeshes[i]);
	}

	// 子ノード
	node->children.reserve(aiNode->mNumChildren);
	for (uint32_t i = 0; i < aiNode->mNumChildren; ++i) {
		// 階層構造
		node->children.push_back(ReadNode(aiNode->mChildren[i]));
	}

	// name
	node->name = aiNode->mName.C_Str();

	// TransformCB
	node->transformCBHandle_ = cbManager_->AllocateTransformCB();

	return node;
}

Matrix4x4 ModelManager::ConvertAssimpMatrixToLHRow(const aiMatrix4x4& m) {
	Matrix4x4 out{};

	// 列から行ベクトル、右手から左手座標系に
	out.m[0][0] = m.a1; out.m[0][1] = m.b1; out.m[0][2] = -m.c1; out.m[0][3] = m.d1;
	out.m[1][0] = m.a2; out.m[1][1] = m.b2; out.m[1][2] = -m.c2; out.m[1][3] = m.d2;
	out.m[2][0] = -m.a3; out.m[2][1] = -m.b3; out.m[2][2] = m.c3; out.m[2][3] = -m.d3;
	out.m[3][0] = m.a4; out.m[3][1] = m.b4; out.m[3][2] = -m.c4; out.m[3][3] = m.d4;

	return out;
}