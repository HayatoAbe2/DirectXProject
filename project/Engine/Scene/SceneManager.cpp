#include "SceneManager.h"
#include "../App/Window.h"
#include "../Graphics/Graphics.h"
#include "../Io/Input.h"
#include "BaseScene.h"
#include "../../application/GameScene.h"
#include "GameContext.h"

SceneManager::SceneManager(GameContext* context) {
	gameContext_ = context;
}

SceneManager::~SceneManager() {
	if (currentScene_) {
		delete currentScene_;
	}
}

void SceneManager::Initialize() {
	currentScene_ = new GameScene();
	currentSceneType_ = Scene::kGame;

	currentScene_->SetGameContext(gameContext_); // 初期化より前
	currentScene_->Initialize();
}

void SceneManager::Update() {
	if (currentScene_) {
		currentScene_->Update();
		if (currentScene_->IsFinished()) {
			// 現在シーン削除
			delete currentScene_;
			currentScene_ = nullptr;

			// シーン切り替え
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
