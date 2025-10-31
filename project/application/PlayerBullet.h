#pragma once
#include "MathUtils.h"

class Model;

class PlayerBullet {
	PlayerBullet(Model* model);
	void Update();
	void Draw();
private:
	// トランスフォーム
	Transform transform_{ {1,1,1},{0,0,0},{0,0,0} };

	// 速度
	Vector3 velocity_{};

	// モデル
	Model* model_ = nullptr;
};

