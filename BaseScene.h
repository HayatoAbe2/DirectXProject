#pragma once
#include <wtypes.h>
class Input;
class Audio;
class Graphics;

/// <summary>
/// 各シーンの基底クラス
/// </summary>
class BaseScene {
public:

	virtual ~BaseScene() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize(Graphics* graphics) = 0;

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update(Input* input,Audio* audio) = 0;
	
	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw(Graphics* graphics) = 0;

	// 終了チェック
	virtual bool isFinished() const { return finished_; };

private:

	// シーンの終了
	bool finished_ = false;
};

