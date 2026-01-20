#include "Player.h"
#include "GameContext.h"
#include "BulletManager.h"
#include "Camera.h"
#include "MapCheck.h"
#include "ItemManager.h"
#include "ImGuiManager.h"
#include "Sprite.h"
#include "ParticleSystem.h"
#include "AssaultRifle.h"
#include "Shotgun.h"
#include "FireBall.h"
#include "Pistol.h"
#include "Wavegun.h"
#include "WeaponStatus.h"

#include <vector>
#include <numbers>
#include <cmath>
#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"
#include "Windows.h"
#include "DirectXMath.h"

Player::~Player() {

}

void Player::Initialize(std::unique_ptr<Model> playerModel, GameContext* context) {
	context_ = context;
	model_ = std::move(playerModel);
	transform_.translate.x = 1;
	transform_.translate.z = 1;

	// 操作
	control_ = context->LoadSprite("Resources/Control/leftClick.png");
	control_->SetSize({ 48,65 });
	control_->SetPosition({ 640 - 24 + 100,710 - 65 });

	dashControl_ = context->LoadSprite("Resources/Control/dash.png");
	dashControl_->SetSize({ 212,32 });
	dashControl_->SetPosition({ 10, 100 });

	equipment_ = context->LoadSprite("Resources/Control/equipmentAssaultRifle.png");
	equipment_->SetSize({ 120,120 });
	equipment_->SetPosition({ 640 - 60,710 - 160 });

	for (int i = 0; i < 3; ++i) {
		enchants_->push_back(context->LoadSprite("Resources/UI/Enchants/damageIncrease.png"));
		enchants_->at(i)->SetSize({ 240,40 });
		enchants_->at(i)->SetPosition({ 640 + 80,float(710 - 180 + i * 40) });
	}

	life_ = std::make_unique<Sprite>();
	life_ = context->LoadSprite("Resources/UI/gauge.png");
	life_->SetSize({ 290,68 });
	life_->SetPosition({ 10,10 });

	direction_ = std::make_unique<Model>();
	direction_ = context_->LoadModel("Resources/Direction", "Direction.obj");
	auto data = direction_->GetMaterial(0)->GetData();
	data.color = { 1,0,0,dirDisplayAlpha_ };
	direction_->GetMaterial(0)->SetData(data);

	moveParticle_ = std::make_unique<ParticleSystem>();
	moveParticle_->Initialize(std::move(context->LoadInstancedModel("Resources/Particle/Fire", "fireEffect.obj", moveParticleNum_)));
	moveParticle_->SetLifeTime(10);
	moveParticle_->SetColor({ 0.6f, 0.6f, 0.6f, 1.0f });
}

void Player::Update(MapCheck* mapCheck, ItemManager* itemManager_, Camera* camera, BulletManager* bulletManager) {
	invincibleTimer_--;
	if (redTime_) {
		redTime_--;
		if (redTime_ <= 0) {
			for (auto& mesh : model_->GetData()->meshes) {
				auto data = model_->GetMaterial(0)->GetData();
				data.color = { 1.0f,1.0f,1.0f,1.0f };
				model_->GetMaterial(0)->SetData(data);
			}
		}
	}

	if (!isFall_) {
		if (isUsingBoost_) {
			// ダッシュ
			velocity_ = boostDir_ * boostSpeed_;

			Vector2 pos = { transform_.translate.x,transform_.translate.z };
			for (int i = 0; i < 4; ++i) {
				pos.x += velocity_.x / 4.0f;
				mapCheck->ResolveCollisionX(pos, radius_, isUsingBoost_);
				pos.y += velocity_.z / 4.0f;
				mapCheck->ResolveCollisionY(pos, radius_, isUsingBoost_);
			}
			transform_.translate.x = pos.x;
			transform_.translate.z = pos.y;

			boostTime_++;

			if (maxBoostTime_ <= boostTime_) { // 終了時
				isUsingBoost_ = false;
				boostTime_ = 0;

				// 落下判定
				if (mapCheck->IsFall({ transform_.translate.x,transform_.translate.z })) {
					isFall_ = true;
					context_->SoundPlay(L"Resources/Sounds/SE/fall.mp3", false);
				}
			}

		} else {
			// 入力方向
			Vector2 input = { 0,0 };

			// 移動
			input.x = context_->GetLeftStick().x;
			input.y = context_->GetLeftStick().y;

			if (context_->IsPress(DIK_A)) {
				input.x = -1;
			}

			if (context_->IsPress(DIK_D)) {
				input.x = 1;
			}

			if (context_->IsPress(DIK_W)) {
				input.y = -1;
			}

			if (context_->IsPress(DIK_S)) {
				input.y = 1;
			}

			// 入力を反映
			Vector2 normalized = Normalize(input);
			velocity_.x = normalized.x * moveSpeed_;
			velocity_.z = -normalized.y * moveSpeed_;

			boostCoolTime_--;

			// ダッシュ入力
			if (context_->IsTrigger(DIK_SPACE) && Length(normalized) > 0.1f && boostCoolTime_ < 0) {
				isUsingBoost_ = true;
				boostDir_ = { normalized.x,0,-normalized.y };
				// ダッシュ前の位置を記憶
				landPos_ = transform_.translate;

				for (int i = 0; i < 5; ++i) {
					Vector3 randomVector = {
					context_->RandomFloat(-moveParticleRange_ / 2.0f, moveParticleRange_ / 2.0f),
					-0.5f,
					context_->RandomFloat(-moveParticleRange_ / 2.0f, moveParticleRange_ / 2.0f),
					};
					Transform transform = model_->GetTransform();
					transform.translate += randomVector;
					transform.scale = { 1.0f,1.0f,1.0f };
					moveParticle_->Emit(transform, -velocity_ * 0.4f);
				}
				moveParticleEmitTimer_ = 0;
			}
		}

		// 速度をもとに移動
		Vector2 pos = { transform_.translate.x,transform_.translate.z };
		for (int i = 0; i < 4; ++i) {
			pos.x += velocity_.x / 4.0f;
			mapCheck->ResolveCollisionX(pos, radius_, isUsingBoost_);
			pos.y += velocity_.z / 4.0f;
			mapCheck->ResolveCollisionY(pos, radius_, isUsingBoost_);
		}
		transform_.translate.x = pos.x;
		transform_.translate.z = pos.y;

		if (velocity_.x != 0 || velocity_.z != 0) {
			// 移動による向き変更
			transform_.rotate.y = -std::atan2(velocity_.z, velocity_.x) + float(std::numbers::pi) / 2.0f;

			// パーティクル
			moveParticleEmitTimer_++;
			if (moveParticleEmitTimer_ >= moveParticleEmitInterval_) {
				for (int i = 0; i < 3; ++i) {
					Vector3 randomVector = {
					context_->RandomFloat(-moveParticleRange_ / 2.0f, moveParticleRange_ / 2.0f),
					-0.5f,
					context_->RandomFloat(-moveParticleRange_ / 2.0f, moveParticleRange_ / 2.0f),
					};
					Transform transform = model_->GetTransform();
					transform.translate += randomVector;
					transform.scale = { 1.0f,1.0f,1.0f };
					moveParticle_->Emit(transform, -velocity_ * 0.4f);
				}
				moveParticleEmitTimer_ = 0;
			}
		}
		moveParticle_->Update();

		// アイテム取得
		if (context_->IsTrigger(DIK_F)) {
			itemManager_->Interact(this);
			if (rangedWeapon_) {

				// 武器アイコン
				if (dynamic_cast<AssaultRifle*>(rangedWeapon_.get())) {
					equipment_ = context_->LoadSprite("Resources/Control/equipmentAssaultRifle.png");
				} else if (dynamic_cast<Pistol*>(rangedWeapon_.get())) {
					equipment_ = context_->LoadSprite("Resources/Control/equipmentPistol.png");
				} else if (dynamic_cast<Shotgun*>(rangedWeapon_.get())) {
					equipment_ = context_->LoadSprite("Resources/Control/equipmentShotgun.png");
				} else if (dynamic_cast<FireBall*>(rangedWeapon_.get())) {
					equipment_ = context_->LoadSprite("Resources/Control/equipmentSpellbook.png");
				} else if (dynamic_cast<Wavegun*>(rangedWeapon_.get())) {
					equipment_ = context_->LoadSprite("Resources/Control/equipmentWavegun.png");
				}
				equipment_->SetSize({ 120, 120 });
				equipment_->SetPosition({ 640 - 60, 710 - 160 });

				// 効果
				for (int i = 0; i < static_cast<int>(rangedWeapon_->GetStatus().rarity); ++i) {
					auto enchant = rangedWeapon_->GetStatus().enchants[i];
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
						enchants_->at(i) = (context_->LoadSprite("Resources/UI/Enchants/IncreaseMovespeed.png"));//
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

		// 攻撃の向き
		if (context_->IsClickLeft()) {
			Vector2 win = context_->GetWindowSize();
			Vector2 mouse = context_->GetMousePosition();
			mouse.y = win.y - mouse.y;

			Vector2 dir = Normalize(mouse - win / 2.0f);
			attackDirection_ = { dir.x,0,dir.y };

			// プレイヤーの向き
			transform_.rotate.y = -std::atan2(attackDirection_.z, attackDirection_.x) + float(std::numbers::pi) / 2.0f;

			// 減速
			if (rangedWeapon_) {
				moveSpeed_ = defaultMoveSpeed_ * (1.0f - rangedWeapon_->GetStatus().weight);
			}
		} else {
			moveSpeed_ = defaultMoveSpeed_;
		}

		if (rangedWeapon_) {
			rangedWeapon_->Update();
			weaponTransform_ = transform_;
			weaponTransform_.translate += {std::sin(transform_.rotate.y), 0, std::cos(transform_.rotate.y)}; // 前方に配置

			// enchant分の速度
			for (auto enchant : rangedWeapon_->GetStatus().enchants) {
				if (enchant == Enchants::moveSpeed) {
					moveSpeed_ *= 1.15f;
				}
			}

		}

		// 射撃
		if (shootCoolTime_ <= 0) {
			if (context_->IsClickLeft()) {
				if (rangedWeapon_) {
					shootCoolTime_ = rangedWeapon_->Shoot(weaponTransform_.translate, attackDirection_, bulletManager, context_, false);

					// 追加効果
					for (auto enchant : rangedWeapon_->GetStatus().enchants) {
						switch (enchant) {
						case static_cast<int>(Enchants::shortCooldown):
							shootCoolTime_ = shootCoolTime_ * 3 / 4;
							break;
						case static_cast<int>(Enchants::extraBullet):
							if (context_->RandomInt(1, 20) <= 3) {
								extraBulletWaitTime_ = 10;
								canShootExtraBullet_ = true;
							}
							break;
						}
					}

					if (canShootExtraBullet_) {
						if (--extraBulletWaitTime_ <= 0) {
							rangedWeapon_->Shoot(weaponTransform_.translate, attackDirection_, bulletManager, context_, false);
							canShootExtraBullet_ = false;
						}
					}
				}
			}
		} else {
			shootCoolTime_--;
		}

		// ノックバック中(操作はさせる)
		if (stunTimer_ > 0) {
			stunTimer_--;

			// ノックバック
			model_->SetScale({ 1,1,1 });
			float length = Length(velocity_);
			length -= 0.05f;
			if (length < 0) { length = 0; }
			velocity_ = Normalize(velocity_) * length;

			// 速度をもとに移動
			Vector2 pos = { transform_.translate.x,transform_.translate.z };
			for (int i = 0; i < 3; ++i) { // 3回に分ける
				pos.x += velocity_.x / 3.0f;
				mapCheck->ResolveCollisionX(pos, radius_, true);
				pos.y += velocity_.z / 3.0f;
				mapCheck->ResolveCollisionY(pos, radius_, true);
			}

			if (stunTimer_ <= 0 && mapCheck->IsFall(pos)) {
				context_->SoundPlay(L"Resources/Sounds/SE/fall.mp3", false);
			}

			transform_.translate = { pos.x,model_->GetTransform().translate.y,pos.y };

			if (rangedWeapon_) {
				weaponTransform_ = transform_;
				weaponTransform_.translate += {std::sin(transform_.rotate.y), 0, std::cos(transform_.rotate.y)}; // 前方に配置
			}
		}

	} else {
		// 落下
		transform_.translate.y -= 1.0f;
		if (transform_.translate.y < -20.0f) {
			hp_ -= maxHp_ / 12.0f;
			invincibleTimer_ = invincibleTime_;

			// その前にいた位置に戻す
			transform_.translate = landPos_;
			isFall_ = false;
			stunTimer_ = 0;
		}
	}
}

void Player::Draw(Camera* camera) {

	model_->SetTransform(transform_);
	context_->DrawModel(model_.get(), camera);
	context_->DrawSprite(dashControl_.get());

	if (rangedWeapon_) {
		rangedWeapon_->GetWeaponModel()->SetTransform(weaponTransform_);
		context_->DrawModel(rangedWeapon_->GetWeaponModel(), camera);
		context_->DrawSprite(control_.get());

		// 照準方向
		direction_->SetTransform(weaponTransform_);
		context_->DrawModel(direction_.get(), camera);

		switch (rangedWeapon_->GetStatus().rarity) {
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
		context_->DrawSprite(equipment_.get());
		for (int i = 0; i < static_cast<int>(rangedWeapon_->GetStatus().rarity); ++i) {
			context_->DrawSprite(enchants_->at(i).get());
		}
	}

	life_->SetTextureRect(0, 0, (hp_ / maxHp_) * 290, 68);
	life_->SetSize({ (hp_ / maxHp_) * 290,68 });
	context_->DrawSprite(life_.get());

	// パーティクル
	context_->DrawParticle(moveParticle_.get(), camera, BlendMode::Add);

#ifdef USE_IMGUI
	ImGui::Begin("Player Info");
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", transform_.translate.x, transform_.translate.y, transform_.translate.z);
	ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", transform_.rotate.x, transform_.rotate.y, transform_.rotate.z);
	ImGui::Text("Scale: (%.2f, %.2f, %.2f)", transform_.scale.x, transform_.scale.y, transform_.scale.z);
	ImGui::Text("HP: %f", hp_);
	ImGui::End();
#endif
}

void Player::Hit(float damage, Vector3 from) {
	if (invincibleTimer_ <= 0) {
		// 軽減の計算
		if (rangedWeapon_) {
			// 追加効果
			for (auto enchant : rangedWeapon_->GetStatus().enchants) {
				switch (enchant) {
				case static_cast<int>(Enchants::resist):
					damage *= 0.8f; // 軽減
					break;
				case static_cast<int>(Enchants::avoid):
					if (context_->RandomInt(1, 10) == 1) {
						damage = 0; // 回避
					}
					break;
				}
			}
		}

		if (damage > 0) {
			hp_ -= damage;
			invincibleTimer_ = invincibleTime_;

			if (hp_ <= 0) {
				// ゲームオーバー

			} else {
				// 行動不能
				stunTimer_ = 10;

				// ノックバック
				velocity_ = Normalize(model_->GetTransform().translate - from) * 0.3f;

				// ダメージを受けたら赤くする
				auto data = model_->GetMaterial(0)->GetData();
				data.color = { 1.0f,0.2f,0.2f,1.0f };
				model_->GetMaterial(0)->SetData(data);
				redTime_ = 2;

				// 被ダメージ時の位置を記憶
				landPos_ = transform_.translate;
			}
		}
	}
}

void Player::SetWeapon(std::unique_ptr<RangedWeapon> rangedWeapon) { rangedWeapon_ = std::move(rangedWeapon); }