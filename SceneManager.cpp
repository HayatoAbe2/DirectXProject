#include "SceneManager.h"
#include "Graphics.h"
#include "BaseScene.h"
#include "Input.h"
#include "Window.h"
#include "GameScene.h"

SceneManager::SceneManager() {
}

SceneManager::~SceneManager() {
	if(currentScene_)
	delete currentScene_;
}

void SceneManager::Initialize(Graphics* graphics) {
	currentScene_ = new GameScene();
	currentScene_->Initialize(graphics);
}

void SceneManager::Update(Input* input) {
	currentScene_->Update(input);
}

void SceneManager::Draw(Graphics* graphics) {
	currentScene_->Draw(graphics);
}

void SceneManager::Finalize() {
}
