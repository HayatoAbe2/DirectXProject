#pragma once
#include "Entity.h"
#include "WeaponStatus.h"
#include "RangedWeapon.h"
#include <MathUtils.h>
#include <vector>
#include <memory>
#include "EnemyStatus.h"

class GameContext;
class MapCheck;
class Camera;
class Player;
class BulletManager;

class Enemy {
public:
	Enemy(std::unique_ptr<Entity> model,Vector3 pos,EnemyStatus status,std::unique_ptr<RangedWeapon> rWeapon);
	// ボス敵
	Enemy(std::unique_ptr<Entity> model,Vector3 pos,EnemyStatus status,std::vector<std::unique_ptr<RangedWeapon>> rWeapons);


	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="context">コンテキスト</param>
	void Update(GameContext* context, MapCheck* mapCheck, Player* player, BulletManager* bulletManager);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="context">コンテキスト</param>
	/// <param name="camera">カメラ</param>
	void Draw(GameContext* context, Camera* camera);

	void Hit(int damage,Vector3 from);

	Transform GetTransform() const { return model_->GetTransform(); }
	float GetRadius() const { return status_.radius; }
	bool IsDead() { return isDead_; }
	bool IsBoss() { return isBoss_; }

private:
	// ボス?
	bool isBoss_ = false;

	// 敵別ステータス
	EnemyStatus status_;

	// 移動
	Vector3 velocity_{};

	// 攻撃の向き
	Vector3 attackDirection_ = {};

	// モデル
	std::unique_ptr<Entity> model_ = nullptr;

	// 武器
	std::unique_ptr<RangedWeapon> rangedWeapon_ = nullptr;
	std::vector<std::unique_ptr<RangedWeapon>> multipleWeapons_; // 複数ある場合
	int weaponChangeTimer_ = 300;
	int weaponNum_ = 0;

	// 射撃クールダウン
	int attackCoolTimer_ = 90;

	// プレイヤー
	Player* target_ = nullptr;

	// 発見範囲
	float searchRadius_ = 8.0f;

	// 見失いカウント
	int loseSightTimer_ = 0;

	// 死亡フラグ
	bool isDead_ = false;

	// スタン時間
	int stunTimer_ = 0;

	// 落下
	bool isFall_ = false;

	// 行動変更タイマー
	int moveTimer_ = 0;
	int stopTimer_ = 0;
	bool isMoving_ = false;

	// 攻撃モーション
	int attackMotionStart_ = 30;
	float EaseIn(float start, float end, float t);

	int overheatCount_ = 0;
	int overheat_ = 4;
};

