#pragma once
#include "../Object/Transform.h"
#include "../Math/MathUtils.h"
class Graphics;
class DebugCamera;

class Camera {
public:

	Transform transform_ = { {1,1,1} };
	Matrix4x4 viewMatrix_;
	Matrix4x4 projectionMatrix_;

	/// <summary>
	/// view,projection行列の更新
	/// </summary>
	/// <param name="graphics"></param>
	/// <param name="debugCamera"></param>
	void UpdateCamera(Vector2 windowSize, DebugCamera& debugCamera);

};