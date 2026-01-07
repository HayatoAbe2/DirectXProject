#pragma once
#include "WeaponStatus.h"
#include "RangedWeapon.h"
#include <MathUtils.h>
#include <vector>
#include <memory>
#include "EnemyStatus.h"
#include "BulletManager.h"
#include "GameContext.h"

class MapCheck;
class Camera;
class Player;

class Enemy {
public:
	Enemy(std::unique_ptr<Model> model,Vector3 pos,EnemyStatus status,std::unique_ptr<RangedWeapon> rWeapon);
	// ボス敵
	Enemy(std::unique_ptr<Model> model,Vector3 pos,EnemyStatus status,std::vector<std::unique_ptr<RangedWeapon>> rWeapons);


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

	// 被ダメージ時
	void Hit(float damage,Vector3 from, const float knockback);

	// 敵ごとの関数
	virtual void Attack(RangedWeapon* rangedWeapon, BulletManager* bulletManager, GameContext* context) = 0;

	Transform GetTransform() const { return model_->GetTransform(); }
	float GetRadius() const { return status_.radius; }
	bool IsDead() { return isDead_; }

protected:

	// 敵別ステータス
	EnemyStatus status_;

	// 移動
	Vector3 velocity_{};

	// 攻撃の向き
	Vector3 attackDirection_ = {};

	// モデル
	std::unique_ptr<Model> model_ = nullptr;

	// 武器
	std::unique_ptr<RangedWeapon> rangedWeapon_ = nullptr;
	std::vector<std::unique_ptr<RangedWeapon>> bossWeapons_; // 複数ある場合

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
};

