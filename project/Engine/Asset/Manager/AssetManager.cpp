#include "AssetManager.h"

AssetManager::AssetManager(DirectXContext* dxContext, Logger* logger) {
	textureManager_ = std::make_unique<TextureManager>(dxContext, logger);
	modelManager_ = std::make_unique<ModelManager>(dxContext, logger, textureManager_.get());
}
