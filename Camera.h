#pragma once
#include "Transform.h"
#include "Matrix4x4.h"
class Graphics;
class DebugCamera;

class Camera {
public:

	Transform transform_;
	Matrix4x4 viewMatrix_;
	Matrix4x4 projectionMatrix_;

	/// <summary>
	/// view,projection行列の更新
	/// </summary>
	/// <param name="graphics"></param>
	/// <param name="debugCamera"></param>
	void UpdateCamera(Graphics& graphics, DebugCamera& debugCamera);

};