#pragma once
#include "dinput.h"
#include "Vector3.h"
#include "Matrix4x4.h"
/// <summary>
/// デバッグカメラ
/// </summary>
class DebugCamera{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="key">キー入力</param>
	void Update(const BYTE* preKey,const BYTE* key, const DIMOUSESTATE& mouse);

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
	void ControlCamera(const BYTE* key, const DIMOUSESTATE& mouse);

	/// <summary>
	/// ビュー行列の更新
	/// </summary>
	void UpdateView();

	// デバッグカメラの有効化
	bool isEnable_ = false;

	// カメラ移動速度
	const float kMoveSpeed_ = 0.1f;
	// カメラ回転速度
	const float kRotateSpeed_ = 0.001f;

	Matrix4x4 rotateMatrix_ = {};

	// ローカル回転角
	Vector3 rotation_ = { 0,0,0 };
	// ローカル座標
	Vector3 translation_ = { 0,0,-50 };
	// ビュー行列
	Matrix4x4 viewMatrix_;
	// 射影行列
	Matrix4x4 orthographicMatrix_;

};

