#pragma once
class ModelManager;
class ModelLoader {
friend class ModelManager; // 該当クラスから関数が利用できる

private:
	Model* LoadObjFile(const std::string& directoryPath, const std::string& filename, Microsoft::WRL::ComPtr<ID3D12Device> device, Graphics& graphics);
	std::string LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
};

