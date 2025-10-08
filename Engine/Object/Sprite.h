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
	~Sprite() {
		if (material_) delete material_;
	}

	///
	/// スプライト読み込み時のSetter
	///
	
	/// <summary>
	/// インデックスリソース設定
	/// </summary>
	/// <param name="resource"></param>
	void SetIndexResource(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource_) { indexResource_ = resource_; }

	/// <summary>
	/// IBV設定
	/// </summary>
	/// <param name="ibv"></param>
	void SetIBV(const D3D12_INDEX_BUFFER_VIEW& ibv) { indexBufferView_ = ibv; }

	/// <summary>
	/// VBV設定
	/// </summary>
	/// <param name="vbv"></param>
	void SetVBV(const D3D12_VERTEX_BUFFER_VIEW& vbv) { vertexBufferView_ = vbv; }

	/// <summary>
	/// 頂点リソース設定
	/// </summary>
	/// <param name="resource"></param>
	void SetVertexResource(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource_) { vertexResource_ = resource_; }

	/// <summary>
	/// 頂点データ設定
	/// </summary>
	/// <param name="vertexData"></param>
	void SetVertexData(VertexData* vertexData) { vertexData_ = vertexData; }

	/// <summary>
	/// 座標変換データ設定
	/// </summary>
	/// <param name="transformMatrix"></param>
	void SetTransformData(TransformationMatrix* transformMatrix) { transformationData_ = transformMatrix; }

	/// <summary>
	/// 座標変換リソース設定
	/// </summary>
	/// <param name="resource"></param>
	void SetTransformResource(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource_) { transformationResource_ = resource_; }

	/// <summary>
	/// material設定
	/// </summary>
	/// <param name="materialResource">マテリアル</param>
	void SetMaterial(Material* material) { 
		if (material_) { delete material_; }
		material_ = material;
	}

	/// <summary>
	/// 大きさ設定
	/// </summary>
	/// <param name="size"></param>
	void SetSize(const Vector3& size) { size_ = size; }

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="graphics">Graphicsインスタンス</param>
	void Draw(Graphics& graphics);

	void UpdateTransform(Camera* camera, float kClientWidth, float kClientHeight, bool useScreenPos = true);

	void ImGuiEdit();

	// アクセサ
	const Transform& GetTransform() const { return transform_; }
	const D3D12_INDEX_BUFFER_VIEW& GetIBV()const { return indexBufferView_; };
	const D3D12_VERTEX_BUFFER_VIEW& GetVBV() const { return vertexBufferView_; };
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSRVHandle() const { return material_->GetTextureSRVHandle(); };
	const D3D12_GPU_VIRTUAL_ADDRESS GetCBV()const {
		return transformationResource_->GetGPUVirtualAddress(); 
	}
	
	void SetTransform(const Transform& transform) {
		transform_ = transform;
	}

private:
	Transform transform_;

	// 描画する時の大きさ
	Vector3 size_;

	Material* material_;

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
