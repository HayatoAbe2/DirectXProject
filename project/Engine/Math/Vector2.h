#pragma once
struct Vector2 {
	float x;
	float y;

	// 演算子オーバーロード
	Vector2& operator*=(float s) {
		x *= s;
		y *= s;
		return *this;
	}

	Vector2& operator-=(const Vector2& v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}

	Vector2& operator+=(const Vector2& v) {
		x += v.x;
		y += v.y;
		return *this;
	}

	Vector2& operator/=(float s) {
		if (s != 0.0f) {
			x /= s;
			y /= s;
		}
		return *this;
	}
};

/// <summary>
/// 2次元ベクトルの長さ
/// </summary>
/// <param name="v">ベクトル</param>
/// <returns>ベクトルの長さ(ノルム)</returns>
float Length(const Vector2& v);

/// <summary>
/// 2次元ベクトルの正規化
/// </summary>
/// <param name="v">ベクトル</param>
/// <returns>長さが1のベクトル</returns>
Vector2 Normalize(const Vector2& v);

// 演算子オーバーロード
Vector2 operator+(const Vector2& v1, const Vector2& v2);
Vector2 operator-(const Vector2& v1, const Vector2& v2);
Vector2 operator*(float scalar, const Vector2& v);
Vector2 operator*(const Vector2& v, float scalar);
Vector2 operator/(const Vector2& v, float scalar);
Vector2 operator-(const Vector2& v);
Vector2 operator+(const Vector2& v);