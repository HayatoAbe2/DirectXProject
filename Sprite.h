#pragma once
#include "Transform.h"
#include "Matrix4x4.h"
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include "Math.h"
#include "Graphics.h"

class Sprite {
public:

	void Initialize(ID3D12Device* device, Graphics* graphics, std::string texturePath);
	void SetPosition(const Vector3& pos);
	void SetScale(const Vector3& scale);
	void SetRotationZ(float angleRadians);

	// アクセサ
	const Transform& GetTransform() const { return transform_; }
	const D3D12_INDEX_BUFFER_VIEW& GetIBV()const { return indexBufferView_; };
	const D3D12_VERTEX_BUFFER_VIEW& GetVBV() const { return vertexBufferView_; };
	const D3D12_GPU_VIRTUAL_ADDRESS GetMaterialAddress()const { return materialResource_->GetGPUVirtualAddress(); };
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetTextureSRVHandle() const { return textureSRVHandleGPU_; };
	const std::string& GetMaterial()const { return texturePath_; };

	void SetTextureSRVHandle(D3D12_GPU_DESCRIPTOR_HANDLE textureSRVHandleGPU) { textureSRVHandleGPU_ = textureSRVHandleGPU; };
	void SetTextureResource(Microsoft::WRL::ComPtr<ID3D12Resource> textureResource) {
		textureResource_ = textureResource;
	}
	void SetTransform(const Transform& transform) {
		transform_ = transform;
	}

private:

	std::string texturePath_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSRVHandleGPU_ = {};
	Transform transform_;

	Material material_;
	Material* materialData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	VertexData* vertexData_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;
	TransformationMatrix* transformationMatrixData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource_ = nullptr;

	friend class Graphics;
};
