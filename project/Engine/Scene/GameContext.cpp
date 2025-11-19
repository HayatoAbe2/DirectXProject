#include "GameContext.h"
#include "../Graphics/Renderer.h"
#include "../Io/Audio.h"
#include "../Io/Input.h"
#include "../Object/ResourceManager.h"
#include "../Object/Entity.h"
#include "../Object/Model.h"
#include "../Object/Sprite.h"
#include "../Object/InstancedModel.h"
#include "../Scene/Camera.h"

GameContext::GameContext(Renderer* renderer, Audio* audio, Input* input,ResourceManager* resourceManager) {
	renderer_ = renderer;
	audio_ = audio;
	input_ = input;
	resourceManager_ = resourceManager;
	
	std::mt19937 randomEngine_(randomDevice_());
}

Vector2 GameContext::GetWindowSize() const {
	RECT rect;
	GetClientRect(input_->GetHwnd(), &rect);
	return { float(rect.right),float(rect.bottom) };
}

std::shared_ptr<Model> GameContext::LoadModel(const std::string& directoryPath, const std::string& filename, bool enableLighting) {
	return resourceManager_->LoadObjFile(directoryPath, filename,enableLighting);
}

std::shared_ptr<InstancedModel> GameContext::LoadInstancedModel(const std::string& directoryPath, const std::string& filename,const int num) {
	return resourceManager_->LoadObjFile(directoryPath, filename,num);
}

std::shared_ptr<Sprite> GameContext::LoadSprite(std::string texturePath) {
	return resourceManager_->LoadSprite(texturePath);
}

void GameContext::DrawEntity(Entity& entity, Camera& camera,BlendMode blendMode) {
	renderer_->DrawEntity(entity, camera,static_cast<int>(blendMode));
}

bool GameContext::IsTrigger(uint8_t keyNumber) {
	return input_->IsTrigger(keyNumber);
}

bool GameContext::IsPress(uint8_t keyNumber) {
	return input_->IsPress(keyNumber);
}

bool GameContext::IsRelease(uint8_t keyNumber) {
	return input_->IsRelease(keyNumber);
}

bool GameContext::IsClickLeft() {
	return input_->IsClickLeft();
}

bool GameContext::IsClickRight() {
	return input_->IsClickRight();
}

bool GameContext::IsClickWheel() {
	return input_->IsClickWheel();
}

bool GameContext::IsTriggerLeft() {
	return input_->IsTriggerLeft();
}

bool GameContext::IsTriggerRight() {
	return input_->IsTriggerRight();
}

bool GameContext::IsTriggerWheel() {
	return input_->IsTriggerWheel();
}

Vector3 GameContext::GetMouseMove() {
	return input_->GetMouseMove();
}

Vector2 GameContext::GetMousePosition() {
	return input_->GetMousePosition();
}

bool GameContext::IsControllerPress(uint8_t buttonNumber) {
	return input_->IsControllerPress(buttonNumber);
}

/// <summary>
/// 左スティック
/// </summary>
/// <returns>-1~1</returns>
Vector2 GameContext::GetLeftStick() {
	return input_->GetLeftStick();
}

/// <summary>
/// 右スティック
/// </summary>
/// <returns>-1~1</returns>
Vector2 GameContext::GetRightStick() {
	return input_->GetRightStick();
}

int GameContext::RandomInt(int min, int max) {
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(randomEngine_);
}

float GameContext::RandomFloat(float min, float max) {
	std::uniform_real_distribution<float> distribution(min,max);
	return distribution(randomEngine_);
}


