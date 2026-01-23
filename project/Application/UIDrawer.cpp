#include "UIDrawer.h"
#include "Player.h"
#include "AssaultRifle.h"
#include "Pistol.h"
#include "Shotgun.h"
#include "FireBall.h"
#include "Wavegun.h"

void UIDrawer::Initialize(GameContext* context,Player* player) {
	context_ = context;
	player_ = player;

	#pragma region PlayerUI
	life_ = std::make_unique<Sprite>();
	life_ = context->LoadSprite("Resources/UI/gauge.png");
	life_->SetSize({ 290,68 });
	life_->SetPosition({ 10,10 });

	equipment_ = context->LoadSprite("Resources/Control/equipmentAssaultRifle.png");
	equipment_->SetSize({ 120,120 });
	equipment_->SetPosition({ 640 - 60,710 - 160 });

	for (int i = 0; i < 3; ++i) {
		enchants_->push_back(context->LoadSprite("Resources/UI/Enchants/damageIncrease.png"));
		enchants_->at(i)->SetSize({ 240,40 });
		enchants_->at(i)->SetPosition({ 640 + 80,float(710 - 180 + i * 40) });
	}

#pragma endregion

	// 操作
	control_ = context->LoadSprite("Resources/Control/leftClick.png");
	control_->SetSize({ 48,65 });
	control_->SetPosition({ 640 - 24 + 100,710 - 65 });

	dashControl_ = context->LoadSprite("Resources/Control/dash.png");
	dashControl_->SetSize({ 212,32 });
	dashControl_->SetPosition({ 10, 100 });
}

void UIDrawer::Update() {
}

void UIDrawer::Draw() {
	// プレイヤーUI
#pragma region PlayerUI
	context_->DrawSprite(dashControl_.get());

	auto weapon = player_->GetWeapon();
	if (weapon != nullptr) {
		context_->DrawSprite(control_.get());
	}

	context_->DrawSprite(equipment_.get());

	float hpRate = player_->GetHP() / player_->GetMaxHP();
	life_->SetTextureRect(0, 0, hpRate * 290, 68);
	life_->SetSize({ hpRate * 290,68 });
	context_->DrawSprite(life_.get());
#pragma endregion

}

void UIDrawer::UpdatePlayerUI() {
	if (player_->GetWeapon() != nullptr) {
		// 所持武器レア度
		auto weapon = player_->GetWeapon();
		switch (weapon->GetStatus().rarity) {
		case static_cast<int>(Rarity::Common):
			equipment_->SetColor({ 0.5f,0.5f,0.5f,1.0f });
			break;
		case static_cast<int>(Rarity::Rare):
			equipment_->SetColor({ 0.1f,0.1f,0.7f,1.0f });
			break;
		case static_cast<int>(Rarity::Epic):
			equipment_->SetColor({ 0.8f,0.1f,0.8f,1.0f });
			break;
		case static_cast<int>(Rarity::Legendary):
			equipment_->SetColor({ 1.0f,0.8f,0.0f,1.0f });
			break;
		}

		// エンチャント
		for (int i = 0; i < static_cast<int>(weapon->GetStatus().rarity); ++i) {
			context_->DrawSprite(enchants_->at(i).get());
		}

		// 武器アイコン
		if (dynamic_cast<AssaultRifle*>(weapon)) {
			equipment_ = context_->LoadSprite("Resources/Control/equipmentAssaultRifle.png");
		} else if (dynamic_cast<Pistol*>(weapon)) {
			equipment_ = context_->LoadSprite("Resources/Control/equipmentPistol.png");
		} else if (dynamic_cast<Shotgun*>(weapon)) {
			equipment_ = context_->LoadSprite("Resources/Control/equipmentShotgun.png");
		} else if (dynamic_cast<FireBall*>(weapon)) {
			equipment_ = context_->LoadSprite("Resources/Control/equipmentSpellbook.png");
		} else if (dynamic_cast<Wavegun*>(weapon)) {
			equipment_ = context_->LoadSprite("Resources/Control/equipmentWavegun.png");
		}
		equipment_->SetSize({ 120, 120 });
		equipment_->SetPosition({ 640 - 60, 710 - 160 });

		// 効果
		for (int i = 0; i < static_cast<int>(weapon->GetStatus().rarity); ++i) {
			auto enchant = weapon->GetStatus().enchants[i];
			switch (enchant) {
			case static_cast<int>(Enchants::increaseDamage):
				enchants_->at(i) = (context_->LoadSprite("Resources/UI/Enchants/damageIncrease.png"));
				break;
			case static_cast<int>(Enchants::bigBullet):
				enchants_->at(i) = (context_->LoadSprite("Resources/UI/Enchants/increaseBulletSize.png"));
				break;
			case static_cast<int>(Enchants::fastBullet):
				enchants_->at(i) = (context_->LoadSprite("Resources/UI/Enchants/increaseBulletSpeed.png"));
				break;
			case static_cast<int>(Enchants::shortCooldown):
				enchants_->at(i) = (context_->LoadSprite("Resources/UI/Enchants/increaseFireRate.png"));
				break;
			case static_cast<int>(Enchants::hardKnockback):
				enchants_->at(i) = (context_->LoadSprite("Resources/UI/Enchants/increaseKnockback.png"));
				break;
			case static_cast<int>(Enchants::extraBullet):
				enchants_->at(i) = (context_->LoadSprite("Resources/UI/Enchants/extraBullet.png"));
				break;
			case static_cast<int>(Enchants::moveSpeed):
				enchants_->at(i) = (context_->LoadSprite("Resources/UI/Enchants/IncreaseMovespeed.png"));
				break;
			case static_cast<int>(Enchants::resist):
				enchants_->at(i) = (context_->LoadSprite("Resources/UI/Enchants/damageResist.png"));
				break;
			case static_cast<int>(Enchants::avoid):
				enchants_->at(i) = (context_->LoadSprite("Resources/UI/Enchants/damageIncrease.png"));// 未使用
				break;
			}
			enchants_->at(i)->SetSize({ 240,40 });
			enchants_->at(i)->SetPosition({ 640 + 80,float(710 - 180 + i * 40) });
		}
	}
}

void UIDrawer::UpdateStageUI() {
}
