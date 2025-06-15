#pragma once

struct Vector3 {
	float x;
	float y;
	float z;
};

// 3次元ベクトル加算
Vector3 Add(const Vector3& v1, const Vector3& v2);

// 3次元ベクトル減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2);

// 3次元ベクトルのスカラー倍
Vector3 Multiply(float scalar, const Vector3& v);

// 3次元ベクトルの内積
float Dot(const Vector3& v1, const Vector3& v2);

// 3次元ベクトルの長さ(ノルム)
float Length(const Vector3& v);

// 3次元ベクトルの正規化
Vector3 Normalize(const Vector3& v);

// 3次元ベクトルのクロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2);
