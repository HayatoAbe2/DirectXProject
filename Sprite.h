#pragma once
#include "Transform.h"
#include "Matrix4x4.h"
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include "Math.h"
#include "Graphics.h"
#include "Camera.h"

class Sprite {
public:

	static Sprite* Initialize(Graphics* graphics, std::string texturePath,Vector2 size);
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="graphics">Graphicsインスタンス</param>
	/// <param name="color">色(デフォルトで白)</param>
	void Draw(Graphics& graphics, const Vector4& color = { 1,1,1,1 });

	void ResetMaterial();
	void UpdateTransform(Camera* camera, float kClientWidth, float kClientHeight);

	// アクセサ
	const Transform& GetTransform() const { return transform_; }
	const D3D12_INDEX_BUFFER_VIEW& GetIBV()const { return indexBufferView_; };
	const D3D12_VERTEX_BUFFER_VIEW& GetVBV() const { return vertexBufferView_; };
	const D3D12_GPU_VIRTUAL_ADDRESS GetMaterialAddress()const { return materialResource_->GetGPUVirtualAddress(); };
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetTextureSRVHandle() const { return textureSRVHandleGPU_; };
	const std::string& GetMaterial()const { return texturePath_; };
	const D3D12_GPU_VIRTUAL_ADDRESS GetCBV()const {
		return transformationResource_->GetGPUVirtualAddress(); }
	const Vector4 GetColor()const { return material_.color; };

	void SetTextureSRVHandle(D3D12_GPU_DESCRIPTOR_HANDLE textureSRVHandleGPU) { textureSRVHandleGPU_ = textureSRVHandleGPU; };
	void SetTextureResource(Microsoft::WRL::ComPtr<ID3D12Resource> textureResource) {
		textureResource_ = textureResource;
	}
	void SetTransform(const Transform& transform) {
		transform_ = transform;
	}
	void SetColor(const Vector4& color) {
		material_.color = color;
	}

private:

	// テクスチャファイルのパス
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
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_;
	TransformationMatrix* transformationData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource_ = nullptr;

	friend class Graphics;
};
