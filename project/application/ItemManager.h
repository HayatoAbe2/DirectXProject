#pragma once
#include "MathUtils.h"
#include "Item.h"
#include <vector>
#include <memory>
#include "Entity.h"

class Player;
class WeaponManager;
class GameContext;
class Camera;

class ItemManager {
public:
	void Initialize(WeaponManager* weaponManager, GameContext* context);
	void Update(Player* player);
	void Draw(GameContext* context, Camera* camera);
	void Interact(Player* player);
	void Spawn(Vector3 pos, GameContext* context, int index);
	
private:
	std::unique_ptr<Entity> control_ = nullptr;
	std::unique_ptr<Entity> particles_ = nullptr;
	WeaponManager* weaponManager_ = nullptr;
	std::vector<std::unique_ptr<Item>> items_;
	bool canInteract_ = true;
};

