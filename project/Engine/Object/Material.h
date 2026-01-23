#pragma once
#include "MaterialData.h"
#include "Texture.h"
#include <wrl.h>
#include <memory>

class ResourceManager;
class Material {
public:

    void Initialize(ResourceManager* rm, bool useTexture, bool enableLighting);
    void UpdateGPU(); // GPUに転送

    void SetData(MaterialData data) { data_ = data; }
	const MaterialData GetData()const { return data_; }

    Microsoft::WRL::ComPtr<ID3D12Resource> GetCBV() { return constantBuffer_; }

	void SetTexture(std::shared_ptr<Texture> texture) { 
		texture_ = texture;
	}
	std::shared_ptr<Texture> GetTexture() { return texture_; }

	void SwitchLighting(bool enableLighting) {
		data_.enableLighting = enableLighting;
	}

	const D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSRVHandle() const {
		if (texture_) {
			return texture_->GetSRVHandle();
		}
		return { 0 };
	}

private:
    MaterialData data_;  // CPU側のデータ
    MaterialData* mappedPtr_ = nullptr; // GPUメモリに直接アクセスするためのポインタ
    Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer_ = nullptr; // GPU側リソース
	std::shared_ptr<Texture> texture_ = nullptr; // テクスチャ
};
