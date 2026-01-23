#pragma once
#include "MathUtils.h"
#include "Item.h"
#include "GameContext.h"
#include "Rarity.h"
#include <vector>
#include <string>
#include <memory>

class Player;
class WeaponManager;
class Camera;

class ItemManager {
public:
	void Initialize(WeaponManager* weaponManager, GameContext* context);
	void Update(Player* player);
	void Draw(Camera* camera);
	void Interact(Player* player);
	void Spawn(Vector3 pos,int index);
	void Spawn(Vector3 pos,int index,Rarity rarity);
	void Drop(Vector3 pos,std::unique_ptr<Weapon> weapon);
	void Reset();
	void LoadCSV(const std::string& filePath,const float tileSize);
	
private:
	GameContext* context_ = nullptr;
	std::unique_ptr<Sprite> control_ = nullptr;
	WeaponManager* weaponManager_ = nullptr;
	std::vector<std::unique_ptr<Item>> items_;
	bool canInteract_ = true;
};

