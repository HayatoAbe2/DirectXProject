#include "GameContext.h"
#include "../Graphics/Renderer.h"
#include "../Io/Audio.h"
#include "../Io/Input.h"
#include "../Object/ResourceManager.h"
#include "../Object/Model.h"
#include "../Object/Sprite.h"

GameContext::GameContext(Renderer* renderer, Audio* audio, Input* input,ResourceManager* resourceManager) {
	renderer_ = renderer;
	audio_ = audio;
	input_ = input;
	resourceManager_ = resourceManager;
}

Vector2 GameContext::GetWindowSize() const {
	return { float(renderer_->GetWindowWidth()),float(renderer_->GetWindowHeight()) };
}

Model* GameContext::LoadModel(const std::string& directoryPath, const std::string& filename) {
	return resourceManager_->LoadObjFile(directoryPath, filename);
}

Sprite* GameContext::LoadSprite(std::string texturePath, Vector2 size) {
	return resourceManager_->LoadSprite(texturePath, size);
}

void GameContext::DrawModel(Model& model, bool useAlphaBlend) {
	renderer_->DrawModel(model, useAlphaBlend);
}

void GameContext::DrawSprite(Sprite& sprite) {
	renderer_->DrawSprite(sprite);
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

bool GameContext::isClickLeft() {
	return input_->isClickLeft();
}

bool GameContext::isClickRight() {
	return input_->isClickRight();
}

bool GameContext::isClickWheel() {
	return input_->isClickWheel();
}

bool GameContext::isTriggerLeft() {
	return input_->isTriggerLeft();
}

bool GameContext::isTriggerRight() {
	return input_->isTriggerRight();
}

bool GameContext::isTriggerWheel() {
	return input_->isTriggerWheel();
}

Vector3 GameContext::GetMouseMove() {
	return input_->GetMouseMove();
}

Vector2 GameContext::GetMousePosition() {
	return input_->GetMousePosition();
}
