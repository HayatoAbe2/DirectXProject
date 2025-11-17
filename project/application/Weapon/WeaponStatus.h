#pragma once
#include <memory>
#include <string>
#include "Model.h"

struct RangedWeaponStatus {
	// ダメージ
	int damage;
	// 射撃中減速
	float weight;
	// 弾の大きさ
	float bulletSize;
	// 弾速
	float bulletSpeed;
	// 射撃クールダウン
	int shootCoolTime;
	// 弾モデル
	std::shared_ptr<Model> bulletModel;
};

struct MeleeWeaponStatus {
	// ダメージ
	int damage;
	// 攻撃中減速
	float weight;
	// 攻撃クールダウン
	int attackCoolTime;
	// 武器モデル
	std::shared_ptr<Model> weaponModel;
};