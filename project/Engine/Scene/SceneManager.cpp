#include "SceneManager.h"
#include "../App/Window.h"
#include "../Graphics/Renderer.h"
#include "../Io/Input.h"
#include "BaseScene.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "GameContext.h"

SceneManager::SceneManager(GameContext* context) {
	gameContext_ = context;
}

void SceneManager::Initialize() {
	//currentScene_ = std::make_unique<TitleScene>();
	//currentSceneType_ = Scene::kTitle;
	currentScene_ = std::make_unique<GameScene>();
	currentSceneType_ = Scene::kGame;

	currentScene_->SetGameContext(gameContext_); // 初期化より前
	currentScene_->Initialize();
}

void SceneManager::Update() {
	if (currentScene_) {
		currentScene_->Update();
		if (currentScene_->IsFinished()) {
			if (currentSceneType_ == Scene::kTitle) {
				// シーン切り替え
				currentScene_ = std::make_unique<GameScene>();
				currentSceneType_ = Scene::kGame;
			} else if (currentSceneType_ == Scene::kGame) {
				// シーン切り替え
				currentScene_ = std::make_unique<TitleScene>();
				currentSceneType_ = Scene::kTitle;
			}

			currentScene_->SetGameContext(gameContext_); // 初期化より前
			currentScene_->Initialize();
			currentScene_->Update();
		}
	}
}

void SceneManager::Draw() {
	if (currentScene_) {
		currentScene_->Draw();
	}
}

void SceneManager::Finalize() {
}
