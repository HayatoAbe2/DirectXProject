#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <dxcapi.h>
#include "../Math/MathUtils.h"
#include "externals/DirectXTex/d3dx12.h"
#include "externals/DirectXTex/DirectXTex.h"

class Texture;
class Model;
class Sprite;
class DescriptorHeapManager;
class CommandListManager;
class Logger;

/// <summary>
/// テクスチャ、モデルなどの管理
/// </summary>
class ResourceManager {
public:
    ~ResourceManager();

    void Initialize(const Microsoft::WRL::ComPtr<ID3D12Device>& device,CommandListManager* commandListManager, DescriptorHeapManager* descriptorHeapManager,Logger* logger);

    DirectX::ScratchImage LoadTexture(const std::string& filePath);

    Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const DirectX::TexMetadata& metadata);

    Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages);

    Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

    Texture* CreateSRV(Texture* texture);

    /// <summary>
    /// モデルのファイル読み込み
    /// </summary>
    /// <param name="directoryPath"></param>
    /// <param name="filename"></param>
    /// <returns></returns>
    Model* LoadObjFile(const std::string& directoryPath, const std::string& filename);

    std::string LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

    Sprite* LoadSprite(std::string texturePath, Vector2 size);

private:
    Logger* logger_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;
	CommandListManager* commandListManager_ = nullptr;
	DescriptorHeapManager* descriptorHeapManager_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> uploadCmdList_;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
 
    // SRVのインデックス
    UINT currentSRVIndex_ = 0;

	// 作成したリソースのキャッシュ
    std::unordered_map<std::string, Texture*> textures_;
    std::unordered_map<std::string, Model*> models_;
    std::unordered_map<std::string, Sprite*> sprites_;
};


