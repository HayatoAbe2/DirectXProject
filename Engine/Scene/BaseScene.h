#pragma once

class Input;
class Audio;
class Graphics;
class ResourceManager;
class GameContext;

/// <summary>
/// 各シーンの基底クラス
/// </summary>
class BaseScene {
public:

	virtual ~BaseScene() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;
	
	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;

	// 終了チェック
	virtual bool IsFinished() const { return finished_; };

	// コンテキストをセット
	virtual void SetGameContext(GameContext* gameContext) { context_ = gameContext; }

protected:
	// コンテキスト
	GameContext* context_ = nullptr;

	// シーンの終了
	bool finished_ = false;
};

