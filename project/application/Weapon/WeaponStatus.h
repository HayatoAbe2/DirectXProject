#pragma once
#include <memory>
#include <string>
class Entity;

struct RangedWeaponStatus {
	// ダメージ
	int damage;
	// 射撃中減速
	float weight;
	// 弾の大きさ
	float bulletRadius;
	// 弾速
	float bulletSpeed;
	// 射撃クールダウン
	int shootCoolTime;
	// 武器モデル
	Entity* weaponModel;
	// 弾読み込みに必要な情報
	std::string bulletDirectoryPath;
	std::string bulletFileName;
};

struct MeleeWeaponStatus {
	// ダメージ
	int damage;
	// 攻撃中減速
	float weight;
	// 攻撃クールダウン
	int attackCoolTime;
	// 武器モデル
	Entity* weaponModel;
};