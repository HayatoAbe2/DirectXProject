#pragma once
#include "Material.h"
#include "VertexData.h"
#include "TransformationMatrix.h"
#include "Texture.h"
#include "Mesh.h"

#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <string>

class Renderer;
class Camera;
class Model {
public:
	///
	/// モデル読み込み時のSetter
	///

	/// <summary>
	/// メッシュ追加
	/// </summary>
	/// <param name="mesh">追加するメッシュ</param>
	void AddMeshes(const std::shared_ptr<Mesh>& mesh) { meshes_.push_back(mesh); }

	const std::string& GetMtlPath()const { return mtlFilePath; }
	
	// メッシュ(全体)の取得
	const std::vector<std::shared_ptr<Mesh>>& GetMeshes() { return meshes_; }

private:
	// モデルデータ
	std::string mtlFilePath;

	// メッシュ
	std::vector<std::shared_ptr<Mesh>> meshes_;
};

