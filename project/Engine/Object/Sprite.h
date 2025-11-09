#pragma once
#include "Transform.h"
#include "Matrix4x4.h"
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include "Math.h"
#include "Renderer.h"
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

	void UpdateTransform(Vector2 windowSize);

	// 切り取り
	void SetTextureRect(float x, float y, float w, float h);

	void ImGuiEdit();

	//
	// アクセサ
	//

	// サイズ
	const Vector2& GetSize() const { return size_; }
	void SetSize(const Vector2& size) { size_ = size; }

	// 位置
	const Vector2& GetPosition() const { return position_; }
	void SetPosition(const Vector2& position) { position_ = position; }

	// 回転
	float GetRotation() const { return rotation_; }
	void SetRotation(float rotation) { rotation_ = rotation; }

	// 色
	const Vector4& GetColor() const { return material_->GetData().color; }
	void SetColor(Vector4 color) { MaterialData data = material_->GetData(); data.color = color; material_->SetData(data); }

	const D3D12_INDEX_BUFFER_VIEW& GetIBV()const { return indexBufferView_; };
	const D3D12_VERTEX_BUFFER_VIEW& GetVBV() const { return vertexBufferView_; };
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSRVHandle() const { return material_->GetTextureSRVHandle(); };
	const D3D12_GPU_VIRTUAL_ADDRESS GetCBV()const {
		return transformationResource_->GetGPUVirtualAddress(); 
	}
	const D3D12_GPU_VIRTUAL_ADDRESS GetMaterialCBV()const {
		return material_->GetCBV()->GetGPUVirtualAddress();
	}

	void UpdateMaterial() { material_->UpdateGPU(); }

private:
	Vector2 size_ = { 640.0f,360.0f };
	Vector2 position_ = {};
	float rotation_ = 0.0f;

	Material* material_;

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	VertexData* vertexData_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_;
	TransformationMatrix* transformationData_;

	friend class Renderer;
};
