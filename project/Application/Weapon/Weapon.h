#pragma once
#include "MathUtils.h"
#include "WeaponStatus.h"
#include <vector>

class BulletManager;
class GameContext;

class Weapon {
public:
	virtual ~Weapon() = default;
	virtual int Shoot(Vector3 pos, Vector3 dir, BulletManager* bulletManager, GameContext* context, Camera* camera, bool isEnemyBullet) = 0;
	virtual void Update() = 0;

	Model* GetWeaponModel() { return model_.get(); }
	Model* GetWeaponShadowModel() { return shadowModel_.get(); }
	const WeaponStatus& GetStatus() const { return status_; }
	bool CanShoot() { return reloadTimer_ <= 0; }
	int GetReloadTimer() { return reloadTimer_; }
	int GetAmmoLeft() { return ammoLeft_; }
	bool IsReloading() { return isReloading_; }

	void StartReload() { isReloading_ = true; }
	
protected:
	WeaponStatus status_;
	std::unique_ptr<Model> model_;
	std::unique_ptr<Model> shadowModel_;
	int ammoLeft_ = 0;
	int reloadTimer_ = 0;
	bool isReloading_ = false;

};

