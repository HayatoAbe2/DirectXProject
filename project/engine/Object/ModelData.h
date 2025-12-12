#pragma once
#include "Mesh.h"
#include <string>
#include <vector>
#include <memory>

// 読み込んだモデルの共通データ
class ModelData {
	// メッシュ
	std::vector<std::unique_ptr<Mesh>> meshes_;

	// マテリアル
	std::vector<std::unique_ptr<Material>> materials_;
};