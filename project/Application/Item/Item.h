#pragma once
#include "Weapon.h"
#include "Rarity.h"
#include <memory>

class GameContext;
class Camera;

class Item {
public:
	~Item();
	Item(std::unique_ptr<Weapon> weapon, Vector3 pos,GameContext* context,Rarity rarity);
	void Draw(GameContext* context, Camera* camera);

	Transform GetTransform() { if (weapon_) { return weapon_->GetWeaponModel()->GetTransform(); } return{}; }
	std::unique_ptr<Weapon> GetWeapon() { return std::move(weapon_); }
	bool IsDead() { return isDead_; }
	void Erase();
private:
	// context
	GameContext* context_;

	// モデル
	std::unique_ptr<Weapon> weapon_ = nullptr;

	// ライトのインデックス
	int lightIndex_ = 0;

	bool isDead_ = false;

	// レア度
	Rarity rarity_{};
};

