#include "TextureManager.h"
#include "Graphics/DirectXContext.h"
#include "Io/Logger.h"
#include "Graphics/GPUData/VertexData.h"
#include "Graphics/CommandListManager.h"
#include "Graphics/DescriptorHeapManager.h"
#include "Graphics/SRVManager.h"
#include "Graphics/BufferManager.h"
#include "Graphics/ConstantBufferManager.h"

TextureManager::TextureManager(DirectXContext* dxContext, Logger* logger) {
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
}

void TextureManager::CreateTextureSRV(const std::shared_ptr<Texture>& texture) {
	if (!texture->GetMtlPath().empty()) {

		// テクスチャのキャッシュ
		auto it = textureCache_.find(texture->GetMtlPath());
		if (it != textureCache_.end()) {
			texture->SetResource(it->second->GetResource());
			texture->SetSRVHandle(it->second->GetSRVHandle());
			return;
		}

		// Textureリソース作成
		DirectX::ScratchImage mipImages = LoadTexture(texture->GetMtlPath());
		const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
		Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = CreateTextureResource(device_, metadata);

		// UploadBufferを保持
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(textureResource, mipImages);
		uploadBuffers_.push_back(intermediateResource);

		// コマンドリストを実行
		commandListManager_->ExecuteAndWait();

		// SRVIndexを進める
		auto index = srvManager_->Allocate();
		// SRV作成
		srvManager_->CreateTextureSRV(index, textureResource.Get(), metadata.format, UINT(metadata.mipLevels));

		// textureResourceをセット
		texture->SetResource(textureResource);
		// SRVのGPUハンドルをセット
		texture->SetSRVHandle(srvManager_->GetGPUHandle(index));

		// キャッシュ登録
		textureCache_[texture->GetMtlPath()] = texture;
	} else {
		texture->SetSRVHandle({ 0 });
		texture->SetResource(nullptr);
	}
}

DirectX::ScratchImage TextureManager::LoadTexture(const std::string& filePath) {
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

Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::CreateTextureResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, const DirectX::TexMetadata& metadata) {
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

Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::UploadTextureData(const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages) {
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	// Subresourceの情報を取得
	DirectX::PrepareUpload(device_, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	// サイズの計算
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
	// IntermediateResourceを作成
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = bufferManager_->CreateUploadBuffer(intermediateSize);

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