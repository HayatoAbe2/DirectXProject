#pragma once
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

class Texture;
class Model;
class Sprite;
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

    Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResourceDefault(size_t sizeInBytes);

    Texture* CreateSRV(Texture* texture);

    Model* CreateInstancingSRV(Model* model, const int numInstance_);
    
    /// <summary>
    /// モデルのファイル読み込み
    /// </summary>
    /// <param name="directoryPath"></param>
    /// <param name="filename"></param>
    /// <returns></returns>
    std::shared_ptr<Model> LoadObjFile(const std::string& directoryPath, const std::string& filename,const int numInstance_ = 1);

    std::string LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

    std::shared_ptr<Sprite> LoadSprite(std::string texturePath);

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
    std::unordered_map<std::string, Texture*> textures_;
    std::unordered_map<std::string, std::shared_ptr<Model>> models_;
    std::unordered_map<std::string, std::shared_ptr<Sprite>> sprites_;
};


