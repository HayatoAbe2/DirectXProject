#pragma once
#include "Entity.h"
#include "RangedWeapon.h"
#include <memory>

class GameContext;
class Camera;

class Item {
public:
	~Item();
	Item(std::unique_ptr<RangedWeapon> rangedWeapon, Vector3 pos,GameContext* context);
	void Draw(GameContext* context, Camera* camera);

	Transform GetTransform() { if (rangedWeapon_) { return rangedWeapon_->GetWeaponModel()->GetTransform(); } return{}; }
	std::unique_ptr<RangedWeapon> GetRangedWeapon() { return std::move(rangedWeapon_); }
	bool IsDead() { return isDead_; }
	void Erase();
private:
	// context
	GameContext* context_;

	// モデル
	std::unique_ptr<RangedWeapon> rangedWeapon_ = nullptr;

	// ライトのインデックス
	int lightIndex_ = 0;

	bool isDead_ = false;
};

