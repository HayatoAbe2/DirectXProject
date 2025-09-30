#include "SceneManager.h"
#include "../Graphics/Graphics.h"
#include "BaseScene.h"
#include "../Io/Input.h"
#include "../App/Window.h"
#include "../../GameScene.h"

SceneManager::SceneManager() {
}

SceneManager::~SceneManager() {
	if (currentScene_) {
		delete currentScene_;
	}
}

void SceneManager::Initialize(Graphics* graphics) {
	currentScene_ = new GameScene();
	currentSceneType_ = Scene::kGame;

	currentScene_->Initialize(graphics);
}

void SceneManager::Update(Input* input, Audio* audio) {
	if (currentScene_) {
		currentScene_->Update(input, audio);
	}
}

void SceneManager::Draw(Graphics* graphics) {
	if (currentScene_) {
		currentScene_->Draw(graphics);
	}
}

void SceneManager::Finalize() {
}
