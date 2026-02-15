#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <memory>

#include "ModelManager.h"
#include "TextureManager.h"

class DirectXContext;
class Logger;

// ゲームリソース管理クラス
class AssetManager {
public:
	// コンストラクタ
	AssetManager(DirectXContext* dxContext, Logger* logger);

	// 読み込み
	std::unique_ptr<Model> LoadModelFile(const std::string& directoryPath, const std::string& filename, bool enableLighting = true) {
		return std::move(modelManager_->Load(directoryPath, filename, enableLighting));
	}
	std::unique_ptr<InstancedModel> LoadModelFile(const std::string& directoryPath, const std::string& filename, const int numInstance, bool enableLighting = true) {
		return std::move(modelManager_->Load(directoryPath, filename, numInstance, enableLighting));
	}


private:
	// モデルマネージャー
	std::unique_ptr<ModelManager> modelManager_ = nullptr;

	// テクスチャマネージャー
	std::unique_ptr<TextureManager> textureManager_ = nullptr;
};

