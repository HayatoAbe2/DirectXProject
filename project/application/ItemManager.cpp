#include "ItemManager.h"
#include "Player.h"
#include "WeaponManager.h"
#include "RangedWeapon.h"
#include "GameContext.h"
#include "Sprite.h"

void ItemManager::Initialize(WeaponManager* weaponManager,GameContext* context) {
	weaponManager_ = weaponManager;

	// 操作
	control_ = std::make_unique<Entity>();
	control_->SetSprite(context->LoadSprite("Resources/Control/f.png"));
	control_->GetSprite()->SetSize({ 30,30 });
	control_->GetSprite()->SetPosition({ 640 - 15,720 - 450 });
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

void ItemManager::Draw(GameContext* context, Camera* camera) {
	for (const auto& item : items_) {
		item->Draw(context, camera);
	}

	if (canInteract_) {
		context->DrawEntity(*control_, *camera);
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
		player->SetWeapon(std::move(items_[closestIndex]->GetRangedWeapon()));
		items_[closestIndex]->Erase();
	}
}

void ItemManager::Spawn(Vector3 pos, GameContext* context, int index) {
	auto rangedWeapon = std::move(weaponManager_->GetRangedWeapon(index));
	auto newItem = std::make_unique<Item>(std::move(rangedWeapon), pos);
	items_.push_back(std::move(newItem));
}
