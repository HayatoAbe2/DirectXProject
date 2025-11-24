#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "Lights/LightsForGPU.h"
#include "vector"

class ResourceManager;
class LightManager {
public:
	void Initialize(ResourceManager* rm);
	
	void Update();

	void DrawImGui();

	#pragma region pointLight
	// ポイントライト追加
	int AddPointLight(ResourceManager* rm) {
		pointLightsData_.push_back(PointLight{});
		Initialize(rm);
		return int(pointLightsData_.size()) - 1; // index
	}

	// ポイントライト削除
	void RemovePointLight(int index,ResourceManager* rm) {
		pointLightsData_.erase(pointLightsData_.begin() + index);
		Initialize(rm);
	}

	// ポイントライト取得(編集用)
	PointLight& GetPointLight(int index) {
		return pointLightsData_[index];
	}
	#pragma endregion

#pragma region spotLight
	// スポットライト追加
	int AddSpotLight(ResourceManager* rm) {
		spotLightsData_.push_back(SpotLight{});
		Initialize(rm);
		return int(spotLightsData_.size()) - 1; // index
	}

	// スポットライト削除
	void RemoveSpotLight(int index, ResourceManager* rm) {
		spotLightsData_.erase(spotLightsData_.begin() + index);
		Initialize(rm);
	}

	// スポットライト取得(編集用)
	SpotLight& GetSpotLight(int index) {
		return spotLightsData_[index];
	}

#pragma endregion
	


	Microsoft::WRL::ComPtr<ID3D12Resource> GetLightResource() { return lightResource_; }


private:

	// ライト全体
	Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_ = nullptr;
	LightsForGPU* lightData_ = nullptr;

	// 各ライト
	DirectionalLight directionalLightData_;
	std::vector<PointLight> pointLightsData_;
	std::vector<SpotLight> spotLightsData_;
};

