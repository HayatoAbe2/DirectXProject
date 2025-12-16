#pragma once
#include "MathUtils.h"
#include "Item.h"
#include "GameContext.h"
#include <vector>
#include <memory>

class Player;
class WeaponManager;
class GameContext;
class Camera;

class ItemManager {
public:
	void Initialize(WeaponManager* weaponManager, GameContext* context);
	void Update(Player* player);
	void Draw(Camera* camera);
	void Interact(Player* player);
	void Spawn(Vector3 pos,int index);
	void Drop(Vector3 pos,std::unique_ptr<RangedWeapon> weapon);
	void Reset();
	
private:
	GameContext* context_ = nullptr;
	std::unique_ptr<Sprite> control_ = nullptr;
	WeaponManager* weaponManager_ = nullptr;
	std::vector<std::unique_ptr<Item>> items_;
	bool canInteract_ = true;
};

