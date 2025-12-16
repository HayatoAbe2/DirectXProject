#pragma once
#include "MathUtils.h"
#include "WeaponStatus.h"
#include <vector>

class BulletManager;
class GameContext;

class MeleeWeapon {
	virtual int Attack(Vector3 pos, Vector3 dir, BulletManager* bulletManager, GameContext* context, bool isEnemyAttack) = 0;
	virtual void Update() = 0;

	Model* GetWeaponModel() { return model_.get(); }
    const MeleeWeaponStatus& GetStatus() const { return status_; }

protected:
	MeleeWeaponStatus status_;
	std::unique_ptr<Model> model_;
};

