#pragma once
#include "WeaponStatus.h"
#include "Weapon.h"
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
	virtual ~Enemy() = default;
	Enemy(std::unique_ptr<Model> model, std::unique_ptr<Model> shadow, Vector3 pos, EnemyStatus status,std::unique_ptr<Weapon> rWeapon);
	// ボス敵
	Enemy(std::unique_ptr<Model> model, std::unique_ptr<Model> shadow, Vector3 pos, EnemyStatus status,std::vector<std::unique_ptr<Weapon>> rWeapons);


	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="context">コンテキスト</param>
	void Update(GameContext* context, MapCheck* mapCheck, Player* player, BulletManager* bulletManager, Camera* camera);

	// Update内関数
	void Wait(GameContext* context);
	void Stun(GameContext* context, MapCheck* mapCheck);
	void Fall();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="context">コンテキスト</param>
	/// <param name="camera">カメラ</param>
	void Draw(GameContext* context, Camera* camera);

	// 被ダメージ時
	void Hit(float damage,Vector3 from, const float knockback);

	// 敵ごとの関数
	virtual void Attack(Weapon* weapon, BulletManager* bulletManager, GameContext* context, Camera* camera) = 0;

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
	std::unique_ptr<Model> shadowModel_ = nullptr;

	// 武器
	std::unique_ptr<Weapon> weapon_ = nullptr;
	std::vector<std::unique_ptr<Weapon>> bossWeapons_; // 複数ある場合

	// 射撃クールダウン
	int attackCoolTimer_ = 90;

	// プレイヤー
	Player* target_ = nullptr;

	// 発見範囲
	float searchRadius_ = 8.0f;

	// 未発見移動
	int randomMoveTime_ = 45; // 動く時間
	const int minRandomMoveTime_ = 30;
	const int maxRandomMoveTime_ = 120;
	int randomStopTime_ = 30; // その後止まる時間
	const int minRandomStopTime_ = 30;
	const int maxRandomStopTime_ = 150;
	int randomTimer_ = 0;
	bool isMoving_ = false;

	// 見失いカウント
	int loseSightTimer_ = 0;

	// 死亡フラグ
	bool isDead_ = false;

	// スタン時間
	int stunTimer_ = 0;
	int hitColorTime_ = 0;

	// 落下
	bool isFall_ = false;

	// 行動変更タイマー
	int rotateTimer_ = 5;
	int rotateTime_ = 5;
	const int minRotateTimer_ = 5;
	const int maxRotateTimer_ = 20;

	// 攻撃モーション
	int attackMotionStart_ = 30;
	float EaseIn(float start, float end, float t);

	// プレイヤーに近づく最小距離
	float minDistance_ = 2.5f;

	// 自動発見
	bool targetAutoFound_ = false;
};

