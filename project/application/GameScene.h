#pragma once
#include "Transform.h"
#include "DebugCamera.h"
#include "Camera.h"
#include "BaseScene.h"

#include <list>
#include <memory>
#include <vector>
#include <random>

class Entity;
class Renderer;
class Camera;
class Player;
class EnemyManager;
class BulletManager;
class EffectManager;
class MapTile;
class MapCheck;
class WeaponManager;
class ItemManager;
class CollisionChecker;

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
	std::unique_ptr<Entity> playerModel_ = nullptr;
	std::unique_ptr<Entity> enemyModel_ = nullptr;
	std::unique_ptr<Entity> wall_ = nullptr;
	std::unique_ptr<Entity> floor_ = nullptr;
	std::unique_ptr<Entity> goal_ = nullptr;
	std::unique_ptr<Entity> skydome_ = nullptr;
	std::unique_ptr<Entity> cloud_ = nullptr;
	std::unique_ptr<Entity> fade_ = nullptr;

	// プレイヤー
	Player* player_ = nullptr;

	// 敵
	EnemyManager* enemyManager_ = nullptr;

	// 弾
	BulletManager* bulletManager_ = nullptr;

	// エフェクト
	EffectManager* effectManager_ = nullptr;

	// マップ
	MapTile* mapTile_ = nullptr;

	// マップ判定
	MapCheck* mapCheck_ = nullptr;

	// 武器マネージャー
	WeaponManager* weaponManager_ = nullptr;

	// アイテムマネージャー
	ItemManager* itemManager_ = nullptr;

	// 当たり判定
	CollisionChecker* collisionChecker_ = nullptr;

	// カメラ
	Camera* camera_ = nullptr;
	float cameraDistance_ = 20.0f;

	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;

	// フェード
	bool isFadeIn_ = true;
	bool isFadeOut_ = false;
	int fadeTimer_ = 0;
	const int kMaxFadeinTimer_ = 30;
	const int kMaxFadeoutTimer_ = 40;

	// 階数
	int currentFloor_ = 1;
};
