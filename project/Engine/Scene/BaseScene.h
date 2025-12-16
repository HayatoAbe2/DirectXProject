#pragma once
#include "GameContext.h"

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
	virtual bool IsFinished() {
		if (isScenefinished_) {
			isScenefinished_ = false;
			return true;
		}
		return false;
	};

	// コンテキストをセット
	virtual void SetGameContext(GameContext* gameContext) { context_ = gameContext; }

	// 次のシーンを取得
	virtual BaseScene* GetNextScene() const { return nextScene_; }

protected:
	// コンテキスト
	GameContext* context_ = nullptr;

	// シーンの終了
	bool isScenefinished_ = false;

	// 次シーン
	BaseScene* nextScene_ = nullptr;
};

