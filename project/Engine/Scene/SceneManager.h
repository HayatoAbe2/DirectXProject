#pragma once
#include "BaseScene.h"
#include <memory>

class Renderer;
class Input;
class Audio;
class GameContext;

/// <summary>
/// シーン管理
/// </summary>
class SceneManager {
public:
	SceneManager(GameContext* context);

	// シーンの初期化
	void Initialize();
	// シーンの更新
	void Update();
	// シーンの描画
	void Draw();
	// シーンの終了
	void Finalize();

private:
	// 現在のシーン
	std::unique_ptr<BaseScene> currentScene_ = nullptr;

	// 次のシーン
	std::unique_ptr<BaseScene> nextScene_ = nullptr;

	enum Scene {
		kTitle,
		kGame,
	};
	
	Scene currentSceneType_ = Scene::kGame;

	GameContext* gameContext_ = nullptr;
};

