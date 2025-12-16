#pragma once
#include "Model.h"

struct RangedWeaponStatus {
	// ダメージ
	int damage;
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
};

struct MeleeWeaponStatus {
	// ダメージ
	int damage;
	// 攻撃中減速
	float weight;
	// 攻撃の前隙
	int chargeTime;
	// 攻撃クールダウン
	int attackCoolTime;
};