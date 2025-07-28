//#include "ModelManager.h"
//#include "Model.h"
//#include "ModelLoader.h"
//
//Model* ModelManager::Load(const std::string& key, const std::string& directoryPath, const std::string& filename) {
//	// キャッシュがあるか検索
//	auto find = modelCache_.find(key);
//	if (find != modelCache_.end()) { // キーが登録済みだった場合
//		// キャッシュにあるものを返す
//		return find->second;
//	}
//
//	// なければ新しく読み込む
//	Model* model = modelLoader_->LoadObjFile(directoryPath, filename);
//	
//	// キャッシュに保存して返す
//	modelCache_[key] = model;
//	return model;
//}
//
//Model* ModelManager::Get(const std::string& key) {
//	return nullptr;
//}
//
//Model* ModelManager::Unload(const std::string& key) {
//	return nullptr;
//}
