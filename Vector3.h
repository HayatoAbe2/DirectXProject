#pragma once

/// <summary>
/// 三次元ベクトル
/// </summary>
struct Vector3 {
	float x;
	float y;
	float z;

	// 演算子オーバーロード
	Vector3& operator*=(float s) {
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	Vector3& operator-=(const Vector3& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	Vector3& operator+=(const Vector3& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector3& operator/=(float s) {
		if (s != 0.0f) {
			x /= s;
			y /= s;
			z /= s;
		}
		return *this;
	}
};

/// <summary>
/// 3次元ベクトルの内積
/// </summary>
/// <param name="v1">ベクトルその1</param>
/// <param name="v2">ベクトルその2</param>
/// <returns>二つのベクトルの内積</returns>
float Dot(const Vector3& v1, const Vector3& v2);

/// <summary>
/// 3次元ベクトルの長さ
/// </summary>
/// <param name="v">ベクトル</param>
/// <returns>ベクトルの長さ(ノルム)</returns>
float Length(const Vector3& v);

/// <summary>
/// 3次元ベクトルの正規化
/// </summary>
/// <param name="v">ベクトル</param>
/// <returns>長さが1のベクトル</returns>
Vector3 Normalize(const Vector3& v);

/// <summary>
/// 3次元ベクトルのクロス積
/// </summary>
/// <param name="v1">ベクトルその1</param>
/// <param name="v2">ベクトルその2</param>
/// <returns>二つのベクトルのクロス積</returns>
Vector3 Cross(const Vector3& v1, const Vector3& v2);

/// <summary>
/// 3次元ベクトルの線形補間
/// </summary>
/// <param name="v1">開始地点(t=0)</param>
/// <param name="v2">終了地点(t=1)</param>
/// <param name="t"></param>
/// <returns>補間後のベクトル</returns>
Vector3 Lerp(const ::Vector3& v1, const ::Vector3& v2, float t);

/// <summary>
/// 3次元ベクトルの球面線形補間
/// </summary>
/// <param name="v1">開始地点(t=0)</param>
/// <param name="v2">終了地点(t=1)</param>
/// <param name="t"></param>
/// <returns>補間後のベクトル</returns>
Vector3 Slerp(const ::Vector3& v1, const ::Vector3& v2, float t);

// 演算子オーバーロード
Vector3 operator+(const Vector3& v1, const Vector3& v2);
Vector3 operator-(const Vector3& v1, const Vector3& v2);
Vector3 operator*(float scalar, const Vector3& v);
Vector3 operator*(const Vector3& v, float scalar);
Vector3 operator/(const Vector3& v, float scalar);
Vector3 operator-(const Vector3& v);
Vector3 operator+(const Vector3& v);
