#include "Vector3.h"
#include <cmath>
#include <algorithm>
#include <vector>
#include <cassert>
#include "Lerp.h"
using namespace DirectX;

float Dot(const Vector3& v1, const Vector3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float Length(const Vector3& v) {
	return sqrtf(Dot(v, v));
}

Vector3 Normalize(const Vector3& v) {
	if (Length(v) <= 0.0f) { return { 0,0,0 }; } // ゼロ除算を回避
	Vector3 result;
	result.x = v.x / Length(v);
	result.y = v.y / Length(v);
	result.z = v.z / Length(v);
	return result;
}

Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
};

Vector3 Lerp(const ::Vector3& v1, const ::Vector3& v2, float t) {
	return { v1.x + (v2.x - v1.x) * t, v1.y + (v2.y - v1.y) * t, v1.z + (v2.z - v1.z) * t }; 
}

Vector3 Slerp(const ::Vector3& v1, const ::Vector3& v2, float t) {
	float dot = (Dot(v1, v2));
	dot = std::clamp(dot, -1.0f, 1.0f);

	// θ角度
	float theta = std::acos(dot);
	// sinθ
	float sinTheta = std::sin(theta);
	// sin(θ(1-t))
	float sinThetaFrom = std::sin((1 - t) * theta);
	// sinθt
	float sinThetaTo = std::sin(t * theta);

	// 正規化補間ベクトル
	::Vector3 normalized;
	if (sinTheta < 1e-5) {
		normalized = Normalize(v1);
	} else {
		normalized = (sinThetaFrom * Normalize(v1) + sinThetaTo * Normalize(v2)) / sinTheta;
	}

	// 補間ベクトルの長さ
	float length = Lerp(Length(v1), Length(v2), t);

	return length * normalized;
}

Vector3 CatmullRom(const ::Vector3& p0, const ::Vector3& p1, const ::Vector3& p2, const ::Vector3& p3, float t) {
	::Vector3 p;
	p = (1.0f / 2.0f) * ((-p0 + 3 * p1 - 3 * p2 + p3) * powf(t, 3.0f) + (2 * p0 - 5 * p1 + 4 * p2 - p3) * powf(t, 2.0f) + (-p0 + p2) * t + 2 * p1);
	return p;
}

Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t) {
	assert(points.size() >= 4);

	// 区間数
	size_t division = points.size() - 1;
	// 区間の長さ
	float areaWidth = 1.0f / division;

	// 始点を0,終点を1としたtを計算
	float t_2 = std::fmod(t, areaWidth) * division;
	t_2 = std::clamp(t_2, 0.0f, 1.0f);

	// 区間のインデックス
	size_t index = static_cast<size_t>(t / areaWidth);
	// 区間インデックスが上限を超えないようにする
	index = (std::min)(index, points.size());

	// 4点分のインデックス
	size_t index0 = index - 1;
	size_t index1 = index;
	size_t index2 = index + 1;
	size_t index3 = index + 2;

	if (index == 0) {
		index0 = index1;
	} else if (index3 >= points.size()) {
		index3 = index2;
		if (index2 >= points.size()) {
			index2 = index1;
			index3 = index1;
		}
	}

	// 4点の座標
	const ::Vector3& p0 = points[index0];
	const ::Vector3& p1 = points[index1];
	const ::Vector3& p2 = points[index2];
	const ::Vector3& p3 = points[index3];

	// 補間
	return CatmullRom(p0, p1, p2, p3, t_2);
}

Vector3 ToVector3(const XMVECTOR& v) {
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, v);
	return Vector3{ temp.x, temp.y, temp.z };
}

/// <summary>
/// 3次元ベクトル加算
/// </summary>
/// <param name="v1">足されるベクトル</param>
/// <param name="v2">足すベクトル</param>
/// <returns>二つのベクトルの和</returns>
Vector3 operator+(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	return result;
}

/// <summary>
/// 3次元ベクトル減算
/// </summary>
/// <param name="v1">引かれるベクトル</param>
/// <param name="v2">引くベクトル</param>
/// <returns>二つのベクトルの差</returns>
Vector3 operator-(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return result;
}

/// <summary>
/// 3次元ベクトルのスカラー倍
/// </summary>
/// <param name="scalar">スカラー</param>
/// <param name="v">ベクトル</param>
/// <returns>各要素をスカラー倍したベクトル</returns>
Vector3 operator*(float scalar, const Vector3& v) {
	Vector3 result;
	result.x = scalar * v.x;
	result.y = scalar * v.y;
	result.z = scalar * v.z;
	return result;
}

/// <summary>
/// 3次元ベクトルのスカラー倍
/// </summary>
/// <param name="v">ベクトル</param>
/// <param name="scalar">スカラー</param>
/// <returns>各要素をスカラー倍したベクトル</returns>
Vector3 operator*(const Vector3& v, float scalar) {
	return scalar * v;
}

/// <summary>
/// 3次元ベクトルのスカラー除算
/// </summary>
/// <param name="v">ベクトル</param>
/// <param name="scalar">スカラー</param>
/// <returns>各要素をスカラーで割ったベクトル</returns>
Vector3 operator/(const Vector3& v, float scalar) {
	return (1.0f / scalar) * v;
}

/// <summary>
/// -ベクトル
/// </summary>
/// <param name="v">ベクトル</param>
/// <returns>各要素に-1を掛ける</returns>
Vector3 operator-(const Vector3& v) {
	return { -v.x, -v.y, -v.z };
}

/// <summary>
/// +ベクトル
/// </summary>
/// <param name="v">ベクトル</param>
/// <returns>ベクトル</returns>
Vector3 operator+(const Vector3& v) {
	return v;
}