#pragma once
#include "Material.h"
#include <vector>
#include "VertexData.h"
#include <d3d12.h>
#include <wrl.h>
#include "TransformationMatrix.h"
#include <string>
class Graphics;
class Camera;
class Model {
public:

	static Model* LoadObjFile(const std::string& directoryPath, const std::string& filename, Graphics& graphics);

	/// <summary>
	/// 描画のため、モデルのトランスフォームデータを更新
	/// </summary>
	/// <param name="camera"></param>
	void UpdateTransformation(Camera& camera);

	// マテリアルを初期化
	void ResetMaterial();

	// 1つのモデルの、複数のトランスフォームを更新する
	void UpdateInstanceTransform(const Transform& transform, const Camera& camera, uint32_t index);

	void ImGuiEdit();

	std::string LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="graphics"></param>
	void Draw(Graphics& graphics, const Vector4& color = {1,1,1,1});

	void EnableInstanceCBV(Graphics& graphics, int maxInstances);

	/// <summary>
	/// 外部CBVをセット(複数のCBVが必要な場合)
	/// </summary>
	/// <param name="address"></param>
	void SetExternalCBV(D3D12_GPU_VIRTUAL_ADDRESS address);

	/// <summary>
	/// 外部CBVを解除
	/// </summary>
	void ClearExternalCBV();

	const D3D12_VERTEX_BUFFER_VIEW& GetVBV()const { return vertexBufferView_; }
	const std::vector<VertexData>& GetVertices() const{ return vertices_; }
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetTextureSRVHandle() const{ return textureSRVHandleGPU_; };
	const D3D12_GPU_VIRTUAL_ADDRESS GetCBV()const { 
		if (useExternalCBV_) {
			return externalCBVAddress_;
		} else { return transformationResource_->GetGPUVirtualAddress(); } };
	const std::string& GetMtlPath()const { return mtlFilePath; };
	const D3D12_GPU_VIRTUAL_ADDRESS GetMaterialAddress()const { return materialResource_->GetGPUVirtualAddress(); }
	const Vector4 GetColor()const { return material_.color; };


	///
	/// Setter
	///

	/// <summary>
	/// SRVのGPUハンドル設定
	/// </summary>
	/// <param name="textureSRVHandleGPU">SRVのハンドル</param>
	void SetTextureSRVHandle(D3D12_GPU_DESCRIPTOR_HANDLE textureSRVHandleGPU) { textureSRVHandleGPU_ = textureSRVHandleGPU; }

	/// <summary>
	/// テクスチャリソース設定
	/// </summary>
	/// <param name="textureResource">テクスチャリソース</param>
	void SetTextureResource(Microsoft::WRL::ComPtr<ID3D12Resource> textureResource) {
		textureResource_ = textureResource;
	}

	/// <summary>
	/// トランスフォーム設定
	/// </summary>
	/// <param name="transform">トランスフォーム</param>
	void SetTransform(const Transform& transform) {
		transform_ = transform;
	}

	/// <summary>
	/// 頂点の追加
	/// </summary>
	/// <param name="vertex">頂点データ</param>
	void AddVertex(const VertexData& vertex) { vertices_.push_back(vertex); }

	/// <summary>
	/// mtlファイルパス設定
	/// </summary>
	/// <param name="mtlPath">mtlファイルのパス</param>
	void SetMtlFilePath(const std::string& mtlPath) { mtlFilePath = mtlPath; }

	/// <summary>
	/// 頂点バッファ設定
	/// </summary>
	/// <param name="vertexBuffer"></param>
	void SetVertexBuffer(const Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer) { vertexBuffer_ = vertexBuffer; }

	/// <summary>
	/// 頂点バッファビュー設定
	/// </summary>
	/// <param name="vertexBufferView"></param>
	void SetVertexBufferView(const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView) { vertexBufferView_ = vertexBufferView; }

	/// <summary>
	/// 色設定
	/// </summary>
	/// <param name="color"></param>
	void SetColor(const Vector4& color) {
		material_.color = color;
	}
private:

	// モデルデータ
	std::vector<VertexData> vertices_;
	std::string mtlFilePath;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	Material* materialData_ = nullptr;
	Material material_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Transform transform_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSRVHandleGPU_ = {};

	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_ = nullptr;
	TransformationMatrix* transformationData_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource_ = nullptr;

	// 複数体描画用（必要なときだけ使う）
	Microsoft::WRL::ComPtr<ID3D12Resource> instanceCBVResource_ = nullptr;
	uint8_t* instanceCBVMappedPtr_ = nullptr;
	UINT instanceCBVStride_ = 0;
	// 外部CBV指定の有無とそのアドレス
	bool useExternalCBV_ = false;
	D3D12_GPU_VIRTUAL_ADDRESS externalCBVAddress_ = 0;
};

