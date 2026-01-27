#pragma once
#include "Model.h"
#include "Rarity.h"

enum Enchants {
	increaseDamage, // ダメージ増加
	bigBullet, // 弾サイズ増加
	fastBullet, // 弾速度増加
	shortCooldown, // 連射速度増加
	hardKnockback, // ノックバック増加
	moveSpeed, // 持っているときの移動速度増加
	resist, // 持っているときの被ダメージ軽減
	extraBullet, // たまに弾が増える
	avoid, // 敵の攻撃を低確率で回避
};
constexpr int enchantsCount = int(Enchants::resist) + 1;

struct WeaponStatus {
	// ダメージ
	float damage;
	// 射撃中減速率
	float weight;
	// 弾の大きさ
	float bulletSize;
	// 弾速
	float bulletSpeed;
	// 射撃クールダウン
	int shootCoolTime;
	// 弾の生存時間
	int bulletLifeTime;
	// ノックバック
	float knockback;
	// 弾数
	int magazineSize;
	// リロード時間
	int reloadTime;

	// エンチャント
	std::vector<Enchants> enchants;
	// レア度
	Rarity rarity;
};
