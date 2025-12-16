#pragma once
#include "Node.h"
#include <d3d12.h>
#include <wrl.h>
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <dxcapi.h>
#include "../Math/MathUtils.h"
#include "../Object/TransformationMatrix.h"
#include "externals/DirectXTex/d3dx12.h"
#include "externals/DirectXTex/DirectXTex.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Texture;
class Mesh;
class Model;
class ModelData;
class Sprite;
class InstancedModel;
class DescriptorHeapManager;
class SRVManager;
class CommandListManager;
class Logger;
class Camera;

/// <summary>
/// テクスチャ、モデルなどの管理
/// </summary>
class ResourceManager {
public:
    ~ResourceManager();

    void Initialize(const Microsoft::WRL::ComPtr<ID3D12Device>& device,CommandListManager* commandListManager, DescriptorHeapManager* descriptorHeapManager,SRVManager* srvManager,Logger* logger);

    DirectX::ScratchImage LoadTexture(const std::string& filePath);

    Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const DirectX::TexMetadata& metadata);

    Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages);

    Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

    void CreateTextureSRV(Texture* texture);

    void CreateInstancingSRV(InstancedModel* model, const int numInstance_);

    Node ReadNode(aiNode* node);

    
    /// <summary>
    /// モデルのファイル読み込み
    /// </summary>
    /// <param name="directoryPath"></param>
    /// <param name="filename"></param>
    /// <returns></returns>
    std::unique_ptr<Model> LoadModelFile(const std::string& directoryPath, const std::string& filename, bool enableLighting = true);
    std::unique_ptr<InstancedModel> LoadModelFile(const std::string& directoryPath, const std::string& filename,const int numInstance_, bool enableLighting = true);

    std::string LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

    std::unique_ptr<Sprite> LoadSprite(std::string texturePath);

    uint32_t AllocateTransformCB() { return nextId_++; }
private:
    Logger* logger_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;
	CommandListManager* commandListManager_ = nullptr;
	DescriptorHeapManager* descriptorHeapManager_ = nullptr;
    SRVManager* srvManager_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> uploadCmdList_;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
 
    // SRVのインデックス
    UINT currentSRVIndex_ = 1; // 0 = ImGui用

	// 作成したリソースのキャッシュ
    std::unordered_map<std::string, std::shared_ptr<Texture>> texturesCache_;
    std::unordered_map<std::string, std::shared_ptr<ModelData>> modelDataCache_;
    std::unordered_map<std::string, std::shared_ptr<InstancedModel>> instancedModels_;

    // id(ハンドルにしたい)
    uint32_t nextId_ = 0;
};


