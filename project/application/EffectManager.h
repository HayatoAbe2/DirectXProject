#pragma once
#include "MathUtils.h"
#include "Entity.h"
#include <memory>

class GameContext;
class Camera;

class EffectManager {
public:
	void Initialize(GameContext* context);
	void Update();
	void Draw(GameContext* context,Camera*camera);

	void SpawnHitEffect(const Vector3& pos);
private:
	GameContext* context_ = nullptr;

	std::vector<std::unique_ptr<Entity>> hitEffect_;
	float hitEffectUpScaleSpeed_ = 0.2f;
};

