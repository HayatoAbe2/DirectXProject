#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "../Object/Transform.h"
#include <cassert>
#include <DirectXMath.h>

struct Matrix4x4 {
	float m[4][4];
};

// 4x4行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);

// 4x4行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);

// 4x4行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

/// <summary>
/// 4x4逆行列
/// </summary>
/// <param name="m">元となる行列</param>
Matrix4x4 Inverse(const Matrix4x4& m);

/// <summary>
/// 4x4転置行列
/// </summary>
/// <param name="m">元となる行列</param>
/// <returns>行と列を入れ替えた行列</returns>
Matrix4x4 Transpose(const Matrix4x4& m);

/// <summary>
/// 4x4単位行列
/// </summary>
/// <returns>対角成分が1、他が0の行列</returns>
Matrix4x4 MakeIdentity4x4();

/// <summary>
/// 4x4拡大縮小行列
/// </summary>
/// <param name="scale">倍率</param>
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

/// <summary>
/// 3D座標変換
/// </summary>
/// <param name="vector">変換するベクトル</param>
/// <param name="matrix">変換に使われる行列</param>
/// <returns>変換後のベクトル</returns>
Vector3 TransformVector(const Vector3& vector, const Matrix4x4 matrix);

/// <summary>
/// 4x4平行移動行列の作成
/// </summary>
/// <param name="translate">移動量</param>
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

/// <summary>
/// X軸回転行列の作成
/// </summary>
/// <param name="radian">回転量(ラジアン)</param>
Matrix4x4 MakeRotateXMatrix(float radian);

/// <summary>
/// Y軸回転行列の作成
/// </summary>
/// <param name="radian">回転量(ラジアン)</param>
Matrix4x4 MakeRotateYMatrix(float radian);

/// <summary>
/// Z軸回転行列の作成
/// </summary>
/// <param name="radian">回転量(ラジアン)</param>
Matrix4x4 MakeRotateZMatrix(float radian);

/// <summary>
/// 4x4アフィン変換行列作成
/// </summary>
/// <param name="scale">拡大縮小</param>
/// <param name="rotate">回転</param>
/// <param name="translate">平行移動</param>
Matrix4x4 MakeAffineMatrix(const Transform& transform);

/// <summary>
/// 透視投影行列作成
/// </summary>
/// <param name="fovY">縦画角</param>
/// <param name="aspectRatio">アスペクト比</param>
/// <param name="nearClip">近平面</param>
/// <param name="farClip">遠平面</param>
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip = 0.1f, float farClip = 1000.0f);

/// <summary>
/// 3D正射影行列作成
/// </summary>
/// <param name="left">左端</param>
/// <param name="top">上端</param>
/// <param name="right">右端</param>
/// <param name="bottom">下端</param>
/// <param name="nearClip">近平面</param>
/// <param name="farClip">遠平面</param>
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip = 0.0f, float farClip = 1000.0f);

/// <summary>
/// ビューポート行列作成
/// </summary>
/// <param name="left">画面左端</param>
/// <param name="top">画面上端</param>
/// <param name="width">画面横幅</param>
/// <param name="height">画面縦幅</param>
/// <param name="minDepth">最小深度</param>
/// <param name="maxDepth">最大深度</param>
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);

/// <summary>
/// viewProjection行列作成
/// </summary>
/// <param name="cameraTransform">カメラトランスフォーム(SRT)</param>
/// <param name="windowSize">画面サイズ(横幅、縦幅)</param>
/// <returns>viewProjection行列</returns>
Matrix4x4 MakeViewProjectionMatrix(Transform cameraTransform, Vector2 windowSize);

/// <summary>
/// viewProjection行列作成
/// </summary>
/// <param name="cameraTransform">カメラトランスフォーム(SRT)</param>
/// <param name="projectionMatrix">プロジェクション行列</param>
/// <returns>viewProjection行列</returns>
Matrix4x4 MakeViewProjectionMatrix(Transform cameraTransform, Matrix4x4 projectionMatrix);

DirectX::XMMATRIX ToXMMATRIX(const Matrix4x4& mat);

// 演算子オーバーロード
Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2);

Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2);

Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);