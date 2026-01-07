#include "GameContext.h"
#include "../Graphics/Renderer.h"
#include "../Io/Audio.h"
#include "../Io/Input.h"
#include "../Object/ResourceManager.h"
#include "../Object/LightManager.h"

GameContext::GameContext(Renderer* renderer, Audio* audio, Input* input, ResourceManager* resourceManager, LightManager* lightManager) {
	renderer_ = renderer;
	audio_ = audio;
	input_ = input;
	resourceManager_ = resourceManager;
	lightManager_ = lightManager;

	std::mt19937 randomEngine(randomDevice_());
	randomEngine_ = randomEngine;

}

Vector2 GameContext::GetWindowSize() const {
	RECT rect;
	GetClientRect(input_->GetHwnd(), &rect);
	return { float(rect.right),float(rect.bottom) };
}

std::unique_ptr<Model> GameContext::LoadModel(const std::string& directoryPath, const std::string& filename, bool enableLighting) {
	return std::move(resourceManager_->LoadModelFile(directoryPath, filename, enableLighting));
}

std::unique_ptr<InstancedModel> GameContext::LoadInstancedModel(const std::string& directoryPath, const std::string& filename, const int num) {
	return std::move(resourceManager_->LoadModelFile(directoryPath, filename, num));
}

std::unique_ptr<Sprite> GameContext::LoadSprite(std::string texturePath) {
	return std::move(resourceManager_->LoadSprite(texturePath));
}

int GameContext::AddPointLight() {
	return lightManager_->AddPointLight();
}

void GameContext::RemovePointLight(int index) {
	lightManager_->RemovePointLight(index);
}

PointLight& GameContext::GetPointLight(int index) {
	return lightManager_->GetPointLight(index);
}

int GameContext::AddSpotLight() {
	return lightManager_->AddSpotLight();
}

void GameContext::RemoveSpotLight(int index) {
	lightManager_->RemoveSpotLight(index);
}

SpotLight& GameContext::GetSpotLight(int index) {
	return lightManager_->GetSpotLight(index);
}

void GameContext::DrawLightImGui() {
	lightManager_->DrawImGui();
}

void GameContext::DrawModel(Model* model, Camera* camera, BlendMode blendMode) {
	renderer_->DrawModel(model, camera, lightManager_, static_cast<int>(blendMode));
}

void GameContext::DrawSprite(Sprite* sprite, BlendMode blendMode) {
	renderer_->DrawSprite(sprite, static_cast<int>(blendMode));
}

void GameContext::DrawInstancedModel(InstancedModel* instancedModel, Camera* camera, BlendMode blendMode) {
	renderer_->DrawModelInstance(instancedModel, camera, lightManager_, static_cast<int>(blendMode));
}

void GameContext::DrawParticle(ParticleSystem* particleSystem, Camera* camera, BlendMode blendMode) {
	renderer_->DrawParticles(particleSystem, camera, static_cast<int>(blendMode));
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

void GameContext::SoundLoad(const wchar_t* filename) {
	audio_->SoundLoad(filename);
}

void GameContext::SoundPlay(const wchar_t* filename, bool isLoop,float volume) {
	audio_->SoundPlay(filename, isLoop,volume);
}

void GameContext::SoundUnload(const wchar_t* filename) {
	audio_->SoundUnload(filename);
}

int GameContext::RandomInt(int min, int max) {
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(randomEngine_);
}

float GameContext::RandomFloat(float min, float max) {
	std::uniform_real_distribution<float> distribution(min, max);
	return distribution(randomEngine_);
}