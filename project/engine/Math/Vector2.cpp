#include "Vector2.h"
#include "cmath"

float Length(const Vector2& v) {
	return sqrtf(powf(v.x,2)+powf(v.y,2));
}

Vector2 Normalize(const Vector2& v) {
	if (Length(v) <= 0.0f) { return { 0,0 }; } // ゼロ除算を回避
	Vector2 result;
	result.x = v.x / Length(v);
	result.y = v.y / Length(v);
	return result;
}

/// <summary>
/// 2次元ベクトル加算
/// </summary>
/// <param name="v1">足されるベクトル</param>
/// <param name="v2">足すベクトル</param>
/// <returns>二つのベクトルの和</returns>
Vector2 operator+(const Vector2& v1, const Vector2& v2) {
	Vector2 result;
	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	return result;
}

/// <summary>
/// 2次元ベクトル減算
/// </summary>
/// <param name="v1">引かれるベクトル</param>
/// <param name="v2">引くベクトル</param>
/// <returns>二つのベクトルの差</returns>
Vector2 operator-(const Vector2& v1, const Vector2& v2) {
	Vector2 result;
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	return result;
}

/// <summary>
/// 2次元ベクトルのスカラー倍
/// </summary>
/// <param name="scalar">スカラー</param>
/// <param name="v">ベクトル</param>
/// <returns>各要素をスカラー倍したベクトル</returns>
Vector2 operator*(float scalar, const Vector2& v) {
	Vector2 result;
	result.x = scalar * v.x;
	result.y = scalar * v.y;
	return result;
}

/// <summary>
/// 2次元ベクトルのスカラー倍
/// </summary>
/// <param name="v">ベクトル</param>
/// <param name="scalar">スカラー</param>
/// <returns>各要素をスカラー倍したベクトル</returns>
Vector2 operator*(const Vector2& v, float scalar) {
	return scalar * v;
}

/// <summary>
/// 2次元ベクトルのスカラー除算
/// </summary>
/// <param name="v">ベクトル</param>
/// <param name="scalar">スカラー</param>
/// <returns>各要素をスカラーで割ったベクトル</returns>
Vector2 operator/(const Vector2& v, float scalar) {
	return (1.0f / scalar) * v;
}

/// <summary>
/// -ベクトル
/// </summary>
/// <param name="v">ベクトル</param>
/// <returns>各要素に-1を掛ける</returns>
Vector2 operator-(const Vector2& v) {
	return { -v.x, -v.y };
}

/// <summary>
/// +ベクトル
/// </summary>
/// <param name="v">ベクトル</param>
/// <returns>ベクトル</returns>
Vector2 operator+(const Vector2& v) {
	return v;
}