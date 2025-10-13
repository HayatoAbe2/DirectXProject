#pragma once
#include "Transform.h"
#include "MathUtils.h"

class Model;
class GameContext;
class Camera;

class Player {
public:
	~Player();

	void Initialize(Model* playerModel);
	void Update(GameContext* context);
	void Draw(GameContext* context, Camera* camera);

private:
	Transform transform_{ {1,1,1},{0,0,0},{0,0,0} };
	Vector2 input_ = { 0,0 };
	Vector3 velocity_ = { 0,0,0 };

	// 移動速度
	float moveSpeed_ = 0.2f;



	Model* model_ = nullptr;
};

