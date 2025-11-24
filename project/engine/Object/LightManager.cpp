#include "LightManager.h"
#include "ResourceManager.h"
#include "ImGuiManager.h"

void LightManager::Initialize(ResourceManager* rm) {
    UINT cbSize = (sizeof(LightsForGPU) + 255) & ~255; // 256 の倍数に丸める
    lightResource_ = rm->CreateBufferResource(cbSize);
    lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData_));
}

void LightManager::Update() {
	// Directionallight
	lightData_->directionalLight = directionalLightData_;

	// PointLight
	int plCount = (int)pointLightsData_.size();
	plCount = (std::min)(plCount, maxPointLights);

	for (int i = 0; i < plCount; i++)
	{
		lightData_->pointLights[i] = pointLightsData_[i];
	}

	// 未使用ライトを無効化
	for (int i = plCount; i < maxPointLights; i++)
	{
		lightData_->pointLights[i].intensity = 0;
	}

    // SpotLight
    int slCount = (int)spotLightsData_.size();
    slCount = (std::min)(slCount, maxSpotLights);

    for (int i = 0; i < slCount; i++)
    {
        lightData_->spotLights[i] = spotLightsData_[i];
    }

    // 未使用ライトを無効化
    for (int i = slCount; i < maxSpotLights; i++)
    {
        lightData_->spotLights[i].intensity = 0;
    }
}

void LightManager::DrawImGui() {
    if (ImGui::CollapsingHeader("DirectionalLight")){
        Vector3 dlDir = directionalLightData_.direction;
        ImGui::DragFloat3("Direction", &dlDir.x, 0.01f);
        directionalLightData_.direction = Normalize(dlDir);
        ImGui::ColorEdit4("Color", &directionalLightData_.color.x);
        ImGui::DragFloat("Intensity", &directionalLightData_.intensity, 0.01f);
    }

    if (ImGui::CollapsingHeader("PointLights")){
        for (int i = 0; i < pointLightsData_.size(); i++){
            ImGui::PushID(i);  // 同じ UI 名でもIDを変える

            if (ImGui::TreeNode(("PointLight " + std::to_string(i)).c_str()))
            {
                auto& pl = pointLightsData_[i];

                ImGui::DragFloat3("Position", &pl.position.x, 0.1f);
                ImGui::ColorEdit4("Color", &pl.color.x);
                ImGui::DragFloat("Intensity", &pl.intensity,0.1f);
                ImGui::DragFloat("Radius", &pl.radius, 0.01f);
                ImGui::DragFloat("Decay", &pl.decay, 0.01f);

                ImGui::TreePop();
            }

            ImGui::PopID();
        }
    }

    if (ImGui::CollapsingHeader("SpotLights")) {
        for (int i = 0; i < spotLightsData_.size(); i++) {
            ImGui::PushID(int(pointLightsData_.size()) + i);

            if (ImGui::TreeNode(("Spot Light " + std::to_string(i)).c_str()))
            {
                auto& sl = spotLightsData_[i];

                ImGui::DragFloat3("Position", &sl.position.x, 0.1f);
                ImGui::ColorEdit4("Color", &sl.color.x);
                ImGui::DragFloat("Intensity", &sl.intensity, 0.1f);
                Vector3 slDir = spotLightsData_[i].direction;
                ImGui::DragFloat3("Direction", &slDir.x, 0.01f);
                spotLightsData_[i].direction = Normalize(slDir);
                ImGui::DragFloat("Distance", &sl.distance,0.1f);
                ImGui::DragFloat("Decay", &sl.decay, 0.01f);
                ImGui::DragFloat("CosAngle", &sl.cosAngle, 0.01f);
                ImGui::DragFloat("CosFalloffStart", &sl.cosFalloffStart, 0.01f);

                ImGui::TreePop();
            }

            ImGui::PopID();
        }
    }
}

