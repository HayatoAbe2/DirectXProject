#include "LightManager.h"
#include "ResourceManager.h"

void LightManager::Initialize(ResourceManager* rm) {
	lightResource_ = rm->CreateBufferResource(sizeof(LightsForGPU));
	lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData_));

	directionalLightData_.direction = Normalize({ 0.2f, -0.6f, 1.5f });
}

void LightManager::Update() {
	// Directional light をコピー
	lightData_->directionalLight = directionalLightData_;

	// Point Lights をコピー
	int count = (int)pointLightsData_.size();
	count = (std::min)(count, maxPointLights);

	for (int i = 0; i < count; i++)
	{
		lightData_->pointLights[i] = pointLightsData_[i];
	}

	// 未使用ライトを無効化
	for (int i = count; i < maxPointLights; i++)
	{
		lightData_->pointLights[i].intensity = 0;
	}

	// 有効数
	lightData_->pointLightCount = count;
}
