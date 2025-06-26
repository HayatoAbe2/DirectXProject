#pragma once
#include "Math.h"
class Camera;
class Player;
/// <summary>
/// カメラコントローラ
/// </summary>
class CameraController {
	// 矩形
	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Camera* camera);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	void SetTarget(Player* target) { target_ = target; };
	void SetMovableArea_(Rect area) { movableArea_ = area; };
	void Reset();

private:

	// カメラ移動範囲
	Rect movableArea_ = {0, 100, 0, 100};
	// カメラの目標座標
	Vector3 endTranslation_;

	// 座標補間割合
	static inline const float kInterpolationRate = 0.1f; // 0 ~ 1
	// 速度掛け率
	static inline const float kVelocityBias = 0.6f;
	// 追従対象の各方向へのカメラ移動範囲
	static inline const Rect moveLimit = {-500, 500, -500, 500};

	// ビュープロジェクション
	Camera *camera_;

	Player* target_ = nullptr;
	Vector3 targetOffset_ = {0, 0, -23.0f}; // カメラのオフセット
};
