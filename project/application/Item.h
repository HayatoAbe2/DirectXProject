#pragma once
#include "Entity.h"
#include "RangedWeapon.h"
#include <memory>

class GameContext;
class Camera;

class Item {
public:
	Item(std::unique_ptr<RangedWeapon> rangedWeapon, Vector3 pos);
	void Draw(GameContext* context, Camera* camera);

	Transform GetTransform() { if (rangedWeapon_) { return rangedWeapon_->GetWeaponModel()->GetTransform(); } return{}; }
	std::unique_ptr<RangedWeapon> GetRangedWeapon() { return std::move(rangedWeapon_); }
	bool IsDead() { return isDead_; }
	void Erase() { isDead_ = true; }
private:
	// モデル
	std::unique_ptr<RangedWeapon> rangedWeapon_ = nullptr;

	bool isDead_ = false;
};

