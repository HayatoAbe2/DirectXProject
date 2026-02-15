#pragma once
#include "Asset/Model/Model.h"
#include "Asset/Model/InstancedModel.h"

#include <unordered_map>
#include <string>
#include <memory>

#include <assimp/scene.h>

class ModelData;
struct ModelNode;
struct SubMesh;

class DirectXContext;
class Logger;
class CommandListManager;
class DescriptorHeapManager;
class SRVManager;
class BufferManager;
class ConstantBufferManager;
class TextureManager;

class ModelManager {
public:
	ModelManager(DirectXContext* dxContext, Logger* logger, TextureManager* textureManager);
	std::unique_ptr<Model> Load(const std::string& directoryPath, const std::string& filename, bool enableLighting = true);
	std::unique_ptr<InstancedModel> Load(const std::string& directoryPath, const std::string& filename, const int numInstance_, bool enableLighting = true);

private:
	// 関数内で使う関数
	std::unique_ptr<ModelNode> ReadNode(aiNode* node);
	SubMesh CreateSubMesh(aiMesh* aiMesh);
	void CreateInstancingSRV(InstancedModel* model, const int numInstance_);
	Matrix4x4 ConvertAssimpMatrixToLHRow(const aiMatrix4x4& m);


	// デバイス
	ID3D12Device* device_ = nullptr;
	// ログ出力クラス
	Logger* logger_ = nullptr;
	// CommandList管理クラス
	CommandListManager* commandListManager_ = nullptr;
	// DescriptorHeap管理クラス
	DescriptorHeapManager* descriptorHeapManager_ = nullptr;
	// SRV管理クラス
	SRVManager* srvManager_ = nullptr;
	// バッファ管理クラス
	BufferManager* bufferManager_ = nullptr;
	// CB管理クラス
	ConstantBufferManager* cbManager_ = nullptr;
	// テクスチャ管理クラス
	TextureManager* textureManager_ = nullptr;

	// モデルデータのキャッシュ
	std::unordered_map<std::string, std::shared_ptr<ModelData>> modelDataCache_;
};

