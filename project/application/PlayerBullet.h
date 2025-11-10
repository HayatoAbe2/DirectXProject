#pragma once
#include "MathUtils.h"

class Entity;
class Camera;

class PlayerBullet {
public:
	PlayerBullet(Entity* model,const Vector3& velocity);
	void Update();
	void Draw(Camera& camera);
private:
	// トランスフォーム
	Transform transform_{ {1,1,1},{0,0,0},{0,0,0} };

	// 速度
	Vector3 velocity_{};

	// モデル
	Entity* model_ = nullptr;
};

