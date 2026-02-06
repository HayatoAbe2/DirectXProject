#pragma once
#include "GameContext.h"

class Player;
class UIDrawer {
public:
	// 初期化
	void Initialize(GameContext* context, Player* player);
	// 更新
	void Update();
	// 描画
	void Draw();

	// プレイヤーUI(所持アイテムなど)
	void UpdatePlayerUI();
	// ステージUI(進捗)
	void UpdateStageUI();

private:
	GameContext* context_ = nullptr;
	Player* player_ = nullptr;

	struct SpriteData {
		Vector2 pos;
		Vector2 size;
	};

	// hp
	std::unique_ptr<Sprite> life_ = nullptr;
	std::unique_ptr<Sprite> damage_ = nullptr; // 減った分
	std::unique_ptr<Sprite> lifeBack_ = nullptr;
	float whiteLife_ = 0;
	float whiteGaugeSpeed_ = 0.5f;

	// 装備
	std::unique_ptr<Sprite> equipAssaultRifle_ = nullptr;
	std::unique_ptr<Sprite> equipPistol_ = nullptr;
	std::unique_ptr<Sprite> equipShotgun_ = nullptr;
	std::unique_ptr<Sprite> equipFireBall_ = nullptr;
	std::unique_ptr<Sprite> equipWavegun_ = nullptr;

	std::unique_ptr<Sprite> enchantDamage_[3]{};
	std::unique_ptr<Sprite> enchantBulletSize_[3]{};
	std::unique_ptr<Sprite> enchantBulletSpeed_[3]{};
	std::unique_ptr<Sprite> enchantFireRate_[3]{};
	std::unique_ptr<Sprite> enchantKnockback_[3]{};
	std::unique_ptr<Sprite> enchantExtraBullet_[3]{};
	std::unique_ptr<Sprite> enchantMoveSpeed_[3]{};
	std::unique_ptr<Sprite> enchantResist_[3]{};

	Sprite* equipment_ = nullptr;
	Sprite* equipment2_ = nullptr;
	Sprite* enchants_[3]{};
	// 弾数(〇とかで表示,8個*2色目とか)
	std::unique_ptr<Sprite> ammo_ = nullptr;
	std::unique_ptr<Sprite> magazine_ = nullptr;
	SpriteData ammoUIData_ = { {640,380}, {100,22} }; 
	// リロード
	std::unique_ptr<Sprite> reloadBar_ = nullptr;
	std::unique_ptr<Sprite> reloadBarBack_ = nullptr;
	SpriteData reloadBarData_ = { {620,330}, { 20, 40 } };

	// 操作
	std::unique_ptr<Sprite> control_ = nullptr;
	SpriteData controlUIData_ = { {640 - 24 + 100,710 - 65}, { 48,65 } };
	std::unique_ptr<Sprite> dashControl_ = nullptr;
	SpriteData dashUIData_ = { {10,100}, { 212,32 } };

	// 階数
	std::unique_ptr<Sprite> currentFloor_ = nullptr;
	SpriteData floorUIData_ = { {640,320}, { 212,32 } };
};

