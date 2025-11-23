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

	// ポイントライト追加
	PointLight& AddPointLight() {
		pointLightsData_.push_back(PointLight{});
		return pointLightsData_.back();
	}

	// ポイントライト削除
	void RemovePointLight(int index) {
		pointLightsData_.erase(pointLightsData_.begin() + index);
	}


	Microsoft::WRL::ComPtr<ID3D12Resource> GetLightResource() { return lightResource_; }


private:

	// ライト全体
	Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_ = nullptr;
	LightsForGPU* lightData_ = nullptr;

	// 各ライト
	DirectionalLight directionalLightData_;
	std::vector<PointLight> pointLightsData_;
};

