#pragma once
#include "ModelData.h"
#include "TransformationMatrix.h"
#include <string>
class Graphics;
class Camera;
class Model {
public:

	/// <summary>
	/// モデルデータ読み込み
	/// </summary>
	/// <param name="directoryPath">objが存在するファイルまでのパス</param>
	/// <param name="filename">ファイル名</param>
	/// <returns></returns>
	static Model* LoadObjFile(const std::string& directoryPath, const std::string& filename,
		Microsoft::WRL::ComPtr<ID3D12Device> device, Graphics& graphics);

	void Draw(Camera& camera, Graphics& graphics);

	const D3D12_VERTEX_BUFFER_VIEW& GetVBV()const { return vertexBufferView_; };
	const std::vector<VertexData>& GetVertices() const{ return vertices_; };
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetTextureSRVHandle() const{ return textureSRVHandleGPU_; };
	const Microsoft::WRL::ComPtr<ID3D12Resource>& GetTransformationResource()const { return transformationResource_; };
	const std::string& GetMaterial()const { return material_; };

	/// <summary>
	/// SRVのGPUハンドル設定
	/// </summary>
	/// <param name="textureSRVHandleGPU">SRVのハンドル</param>
	void SetTextureSRVHandle(D3D12_GPU_DESCRIPTOR_HANDLE textureSRVHandleGPU) { textureSRVHandleGPU_ = textureSRVHandleGPU; };

	void SetTextureResource(Microsoft::WRL::ComPtr<ID3D12Resource> textureResource) {
		textureResource_ = textureResource;
	}


private:

	/// <summary>
	/// mtlファイル
	/// </summary>
	/// <param name="directoryPath">objが存在するファイルまでのパス</param>
	/// <param name="filename">ファイル名</param>
	/// <returns>マテリアルデータ</returns>
	static std::string LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// 描画のため、モデルのトランスフォームデータを更新
	/// </summary>
	/// <param name="camera"></param>
	void UpdateModel(Camera& camera);

	// モデルデータ
	std::vector<VertexData> vertices_;
	std::string material_;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	Transform transform_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSRVHandleGPU_ = {};

	Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_;
	TransformationMatrix* transformationData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource_ = nullptr;
};

