#pragma once
#include "ModelData.h"

/// <summary>
/// 天球
/// </summary>
class Skydome {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ModelData* model);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	const Transform& GetTransform() { return worldTransform_; }

private:
	// ワールド変換データ
	Transform worldTransform_;
	// モデル
	ModelData* model_ = nullptr;
};
