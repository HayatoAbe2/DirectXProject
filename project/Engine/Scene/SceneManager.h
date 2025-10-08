#pragma once

class Graphics;
class BaseScene;
class Input;
class Audio;
class GameContext;

/// <summary>
/// シーン管理
/// </summary>
class SceneManager {
public:
	SceneManager(GameContext* context);
	~SceneManager();

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
	BaseScene* currentScene_ = nullptr;

	enum Scene {
		kTitle,
		kGame,
	};
	
	Scene currentSceneType_ = Scene::kGame;

	GameContext* gameContext_ = nullptr;
};

