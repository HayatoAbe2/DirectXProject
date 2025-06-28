#pragma once

struct Vector3 {
	float x;
	float y;
	float z;

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

// 演算子オーバーロード
Vector3 operator+(const Vector3& v1, const Vector3& v2);

Vector3 operator-(const Vector3& v1, const Vector3& v2);

Vector3 operator*(float scalar, const Vector3& v);

Vector3 operator*(const Vector3& v, float scalar);

Vector3 operator/(const Vector3& v, float scalar);

Vector3 operator-(const Vector3& v);

Vector3 operator+(const Vector3& v);
