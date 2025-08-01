#pragma once

class Graphics;
class BaseScene;
class Input;
class Audio;

/// <summary>
/// シーン管理
/// </summary>
class SceneManager {
public:
	SceneManager();
	~SceneManager();

	// シーンの初期化
	void Initialize(Graphics* graphics);
	// シーンの更新
	void Update(Input* input,Audio* audio);
	// シーンの描画
	void Draw(Graphics* graphics);
	// シーンの終了
	void Finalize();

private:
	// 現在のシーン
	BaseScene* currentScene_ = nullptr;
};

