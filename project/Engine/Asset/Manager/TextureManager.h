#pragma once
#include "Asset/Texture.h"
#include <memory>
#include <unordered_map>

#include "externals/DirectXTex/d3dx12.h"
#include "externals/DirectXTex/DirectXTex.h"

class DirectXContext;
class Logger;
class CommandListManager;
class DescriptorHeapManager;
class SRVManager;
class BufferManager;
class ConstantBufferManager;
class TextureManager {
public:
	TextureManager(DirectXContext* dxContext, Logger* logger);
	void CreateTextureSRV(const std::shared_ptr<Texture>& texture);

private:
	// 関数内で使う関数
	DirectX::ScratchImage LoadTexture(const std::string& filePath);
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const DirectX::TexMetadata& metadata);
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages);

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

	// テクスチャのキャッシュ
	std::unordered_map<std::string, std::shared_ptr<Texture>> textureCache_;

	// バッファリソース
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> uploadBuffers_;
};

