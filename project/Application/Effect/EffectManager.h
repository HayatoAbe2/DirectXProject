#pragma once
#include "MathUtils.h"
#include <memory>
#include "GameContext.h"

class Camera;

// 敵や弾が消えたあとで続くエフェクト/パーティクルの管理
class EffectManager {
public:
	void Initialize(GameContext* context);
	void Update();
	void Draw(GameContext* context,Camera*camera);

	void SpawnHitEffect(const Vector3& pos);
	void SpawnSoulEffect(const Vector3& spawnPos, const Vector3& goalPos);
private:
	GameContext* context_ = nullptr;

	std::vector<std::unique_ptr<Model>> hitEffect_;
	float hitEffectUpScaleSpeed_ = 0.2f;

	
};

