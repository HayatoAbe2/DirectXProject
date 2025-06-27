#pragma once
#include "Model.h"

/// <summary>
/// 天球
/// </summary>
class Skydome {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	void Draw(Camera& camera, Graphics& graphics);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	const Transform& GetTransform() { return worldTransform_; }

private:
	// ワールド変換データ
	Transform worldTransform_;
	// モデル
	Model* model_ = nullptr;
};
