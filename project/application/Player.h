#pragma once
#include "Transform.h"
#include "MathUtils.h"
#include <vector>
#include <memory>

class Entity;
class GameContext;
class Camera;
class PlayerBullet;

class Player {
public:
	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="playerModel">モデル</param>
	void Initialize(Entity* playerModel);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="context">コンテキスト</param>
	void Update(GameContext* context);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="context">コンテキスト</param>
	/// <param name="camera">カメラ</param>
	void Draw(GameContext* context, Camera* camera);

private:
	// トランスフォーム
	Transform transform_{ {1,1,1},{0,0,0},{0,0,0} };
	
	// 入力方向
	Vector2 input_ = { 0,0 };

	// 速度
	Vector3 velocity_ = { 0,0,0 };

	// 移動速さ
	float moveSpeed_ = 0.2f;

	// モデル
	Entity* model_ = nullptr;

	// 弾
	//std::vector<std::unique_ptr<PlayerBullet>> bullets_;

	// 射撃
	int shootCount_ = 0;
	const int shootTime_ = 5;
};

