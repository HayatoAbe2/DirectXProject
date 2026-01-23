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

	// hp
	std::unique_ptr<Sprite> life_ = nullptr;

	// 装備
	std::unique_ptr<Sprite> equipment_ = nullptr;
	std::vector<std::unique_ptr<Sprite>> enchants_[3]{};

	// 操作
	std::unique_ptr<Sprite> control_ = nullptr;
	std::unique_ptr<Sprite> dashControl_ = nullptr;

};

