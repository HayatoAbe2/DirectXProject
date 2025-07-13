#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"
#include "Math.h"
#include "Input.h"
/// <summary>
/// デバッグカメラ
/// </summary>
class DebugCamera {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="key">キー入力</param>
	void Update(Input* input);

	/// <summary>
	/// ビュー行列
	/// </summary>
	Matrix4x4 GetViewMatrix() { return viewMatrix_; };

	bool IsEnable() { return isEnable_; };
	void SetEnable(bool isEnable) { isEnable_ = isEnable; };

private:
	/// <summary>
	/// カメラの移動・回転
	/// </summary>
	/// <param name="key">キー入力</param>
	void ControlCamera(Input* input);

	/// <summary>
	/// ビュー行列の更新
	/// </summary>
	void UpdateView();

	// デバッグカメラの有効化
	bool isEnable_ = false;

	// カメラ移動速度
	const float kMoveSpeed_ = 0.015f;
	// カメラ回転速度
	const float kRotateSpeed_ = 0.003f;

	// 累積回転行列
	Matrix4x4 matRot_;

	// 注視点（ワールド原点スタート）
	Vector3 target_ = { 0, 0, 0 };
	// 注視点からの距離
	float   distance_ = 50.0f;
	// オービット角度（ラジアン）
	float   yaw_ = 0.0f;  // 水平（Y軸回転）
	float   pitch_ = 0.0f;  // 垂直（ローカルX軸回転）

	// ローカル座標
	Vector3 translation_ = { 0,0,-50 };
	// ビュー行列
	Matrix4x4 viewMatrix_;
	// 射影行列
	Matrix4x4 orthographicMatrix_;
};

