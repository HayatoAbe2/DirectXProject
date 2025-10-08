#include "Material.h"
#include "ResourceManager.h"

void Material::Initialize(ResourceManager* rm,bool useTexture,bool enableLighting) {
	HRESULT hr;
	constantBuffer_ = rm->CreateBufferResource(sizeof(MaterialData));
	hr = constantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedPtr_));
	assert(SUCCEEDED(hr));
	data_.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	data_.useTexture = useTexture;
	data_.enableLighting = enableLighting;
	data_.uvTransform = MakeIdentity4x4();
}

void Material::UpdateGPU() {
	memcpy(mappedPtr_, &data_, sizeof(MaterialData));
}