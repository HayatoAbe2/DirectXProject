#pragma once
#include <wtypes.h>
class Input;
class Graphics;

/// <summary>
/// 各シーンの基底クラス
/// </summary>
class BaseScene {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update(Input* input) = 0;
	
	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw(Graphics* graphics) = 0;
};

