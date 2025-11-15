#pragma once
#include "MaterialData.h"
#include "Texture.h"
#include <wrl.h>

class ResourceManager;
class Material {
public:
	~Material() { if (texture_)delete texture_; }

    void Initialize(ResourceManager* rm, bool useTexture, bool enableLighting);
    void UpdateGPU(); // GPUに転送

    void SetData(const MaterialData& data) { data_ = data; }
	const MaterialData GetData()const { return data_; }

    Microsoft::WRL::ComPtr<ID3D12Resource> GetCBV() { return constantBuffer_; }

	void SetTexture(Texture* texture) { 
		if (texture_) { delete  texture_; }
		texture_ = texture;
	}
	const Texture* GetTexture() const { return texture_; }

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
	Texture* texture_ = nullptr; // テクスチャ
};
