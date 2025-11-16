#pragma once
#include "Entity.h"
#include "WeaponStatus.h"
#include <MathUtils.h>
#include <vector>
#include <memory>

class GameContext;
class MapCheck;
class Camera;
class Player;
class Bullet;

class Enemy {
public:
	Enemy(std::unique_ptr<Entity> model,Vector3 pos);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="context">コンテキスト</param>
	void Update(GameContext* context, MapCheck* mapCheck, Player* player,std::vector<std::unique_ptr<Bullet>>& bullets);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="context">コンテキスト</param>
	/// <param name="camera">カメラ</param>
	void Draw(GameContext* context, Camera* camera);

private:
	// 半径
	float radius_ = 0.5f;

	// 移動の速さ
	float moveSpeed_ = 0.2f;

	// 攻撃の向き
	Vector3 attackDirection_ = {};

	// モデル
	std::unique_ptr<Entity> model_ = nullptr;

	// 弾の性質
	RangedWeaponStatus bulletStatus_;

	// 射撃クールダウン
	int attackCoolTime_ = 0;
	int attackCoolTimer_ = 90;

	// 弾速
	float bulletSpeed_ = 0.1f;

	// プレイヤー
	Player* target_ = nullptr;

	// 検知範囲
	float searchRadius_ = 5.0f;
};

