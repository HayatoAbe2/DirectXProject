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
	int AddPointLight() {
		for (int i = 0; i < maxPointLights; i++) {
			if (isPointFree[i]) {
				isPointFree[i] = false;
				return i;      // ライトID
			}
		}
		return -1; // 空きなし
	}

	// ポイントライト削除
	void RemovePointLight(int index) {
		isPointFree[index] = true;
	}

	// ポイントライト取得(編集用)
	PointLight& GetPointLight(int index) {
		return pointLightsData_[index];
	}
	#pragma endregion

#pragma region spotLight
	// スポットライト追加
	int AddSpotLight() {
		for (int i = 0; i < maxSpotLights; i++) {
			if (isSpotFree[i]) {
				isSpotFree[i] = false;
				return i;      // ライトID
			}
		}
		return -1; // 空きなし
	}

	// スポットライト削除
	void RemoveSpotLight(int index) {
		isSpotFree[index] = true;
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

	// 空き
	bool isPointFree[maxPointLights]{};
	bool isSpotFree[maxSpotLights]{};
};

