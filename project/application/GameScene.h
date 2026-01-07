#pragma once
#include "Transform.h"
#include "DebugCamera.h"
#include "Camera.h"
#include "BaseScene.h"

#include "Player.h"
#include "EnemyManager.h"
#include "BulletManager.h"
#include "EffectManager.h"
#include "MapTile.h"
#include "MapCheck.h"
#include "WeaponManager.h"
#include "ItemManager.h"
#include "CollisionChecker.h"

#include <list>
#include <memory>
#include <vector>
#include <random>

// ゲームシーン
class GameScene : public BaseScene {
public:

	~GameScene() override;

	// 初期化
	void Initialize() override;

	// 更新
	void Update() override;

	// 描画
	void Draw() override;

	// リセット
	void Reset();

private:
	// 3Dモデル
	std::unique_ptr<Model> playerModel_ = nullptr;
	std::unique_ptr<Model> enemyModel_ = nullptr;
	std::unique_ptr<InstancedModel> wall_ = nullptr;
	std::unique_ptr<InstancedModel> floor_ = nullptr;
	std::unique_ptr<Model> goal_ = nullptr;
	std::unique_ptr<Model> skydome_ = nullptr;
	std::unique_ptr<Model> cloud_ = nullptr;
	std::unique_ptr<Sprite> fade_ = nullptr;

	std::unique_ptr<Sprite> resultBG_ = nullptr;
	std::unique_ptr<Sprite> resultCursor_ = nullptr;

	// プレイヤー
	std::unique_ptr<Player> player_ = nullptr;

	// 敵
	std::unique_ptr<EnemyManager> enemyManager_ = nullptr;

	// 弾
	std::unique_ptr<BulletManager> bulletManager_ = nullptr;

	// エフェクト
	std::unique_ptr<EffectManager> effectManager_ = nullptr;

	// マップ
	std::unique_ptr<MapTile> mapTile_ = nullptr;

	// マップ判定
	std::unique_ptr<MapCheck> mapCheck_ = nullptr;

	// 武器マネージャー
	std::unique_ptr<WeaponManager> weaponManager_ = nullptr;

	// アイテムマネージャー
	std::unique_ptr<ItemManager> itemManager_ = nullptr;

	// 当たり判定
	std::unique_ptr<CollisionChecker> collisionChecker_ = nullptr;

	// カメラ
	std::unique_ptr<Camera> camera_ = nullptr;
	float cameraDistance_ = 20.0f;

	// デバッグカメラ
	std::unique_ptr <DebugCamera> debugCamera_ = nullptr;

	// フェード
	bool isFadeIn_ = true;
	bool isFadeOut_ = false;
	int fadeTimer_ = 0;
	const int kMaxFadeinTimer_ = 30;
	const int kMaxFadeoutTimer_ = 40;

	// 階数
	int currentFloor_ = 1;

	bool isShowResult_ = false;
	float resultTime_ = 0;
	float resultArrowMove_ = 0;
};
