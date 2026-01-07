#include "ItemManager.h"
#include "Player.h"
#include "WeaponManager.h"
#include "RangedWeapon.h"
#include "GameContext.h"
#include "Sprite.h"
#include "ParticleSystem.h"
#include <fstream>
#include <sstream>

void ItemManager::Initialize(WeaponManager* weaponManager, GameContext* context) {
	weaponManager_ = weaponManager;
	context_ = context;

	// 操作
	control_ = std::make_unique<Sprite>();
	control_ = context->LoadSprite("Resources/Control/pickup.png");
	control_->SetSize({ 96,39 });
	control_->SetPosition({ 640 - 48,720 - 450 });
}

void ItemManager::Update(Player* player) {
	items_.erase(
		std::remove_if(items_.begin(), items_.end(),
			[](const std::unique_ptr<Item>& item) {
				return item->IsDead();
			}
		),
		items_.end()
	);

	// 最短のアイテムを探す
	int closestIndex = -1;
	float closestDistance = FLT_MAX;
	for (auto& item : items_) {
		float distance = Length(item->GetTransform().translate - player->GetTransform().translate);
		if (distance < closestDistance && distance < 2.0f) {
			closestDistance = distance;
			closestIndex = int(&item - &items_[0]);
		}
	}

	// 取得範囲内
	if (player->GetInteractRadius() >= closestDistance) {
		canInteract_ = true;
	} else {
		canInteract_ = false;
	}
}

void ItemManager::Draw(Camera* camera) {
	for (const auto& item : items_) {
		item->Draw(context_, camera);
	}

	if (canInteract_) {
		context_->DrawSprite(control_.get());
	}
}

void ItemManager::Interact(Player* player) {
	// 最短のアイテムを探す
	int closestIndex = -1;
	float closestDistance = FLT_MAX;
	for (auto& item : items_) {
		float distance = Length(item->GetTransform().translate - player->GetTransform().translate);
		if (distance < closestDistance && distance < 2.0f) {
			closestDistance = distance;
			closestIndex = int(&item - &items_[0]);
		}
	}

	// 取得範囲内なら取る
	if (player->GetInteractRadius() >= closestDistance) {
		Drop(player->GetTransform().translate, player->DropRangedWeapon());
		player->SetWeapon(std::move(items_[closestIndex]->GetRangedWeapon()));
		items_[closestIndex]->Erase();
	}
}

void ItemManager::Spawn(Vector3 pos, int index) {
	// レア度ランダム
	Rarity rarity{};
	int random = context_->RandomInt(1, 200);
	if (random < 80) {
		rarity = Rarity::Common;
	} else if (random < 140) {
		rarity = Rarity::Rare;
	} else if (random < 180) {
		rarity = Rarity::Epic;
	} else {
		rarity = Rarity::Legendary;
	}
	Spawn(pos, index, rarity);
}

void ItemManager::Spawn(Vector3 pos, int index, Rarity rarity) {
	// 設置する
	auto rangedWeapon = std::move(weaponManager_->GetRangedWeapon(index, rarity));
	auto newItem = std::make_unique<Item>(std::move(rangedWeapon), pos, context_, rarity);
	items_.push_back(std::move(newItem));
}

void ItemManager::Drop(Vector3 pos, std::unique_ptr<RangedWeapon> weapon) {
	// アイテムを落とす
	if (weapon) {
		auto newItem = std::make_unique<Item>(std::move(weapon), pos, context_,weapon->GetStatus().rarity);
		items_.push_back(std::move(newItem));
	}
}

void ItemManager::Reset() {
	items_.clear();
}

void ItemManager::LoadCSV(const std::string& filePath, const float tileSize) {
	std::ifstream file(filePath);
	std::string line;

	assert(file.is_open());

	std::getline(file, line); // 最初の行をスキップ

	while (std::getline(file, line)) {
		std::stringstream ss(line);
		std::string itemStr, xStr, zStr;

		std::getline(ss, itemStr, ',');
		std::getline(ss, xStr, ',');
		std::getline(ss, zStr, ',');

		int itemNum = std::stoi(itemStr);
		float x = std::stof(xStr);
		float z = std::stof(zStr);

		Vector3 pos = Vector3{ x * tileSize, 0, z * tileSize };
		Spawn(pos, itemNum);
	}
}
