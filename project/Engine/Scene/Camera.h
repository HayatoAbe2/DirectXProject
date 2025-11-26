#pragma once
#include "../Object/Transform.h"
#include "../Math/MathUtils.h"
class Renderer;
class DebugCamera;
class GameContext;

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
	void UpdateCamera(GameContext* context, DebugCamera& debugCamera);

	void StartShake(float amplitude, int frame);
private:
	Vector3 shake_{};
	int shakeFrame_ = 0;
	int shakeEndFrame_;
	float amplitude_ = 0;
};