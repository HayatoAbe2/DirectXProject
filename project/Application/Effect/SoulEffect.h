#pragma once
#include "GameContext.h"
class SoulEffect {
public:
	// 初期化
	void Initialize(GameContext* context, Vector3 pos, Vector3 goal);
	// 更新
	void Update();
	// 描画
	void Draw(Camera* camera);

private:
	// パーティクル
	std::unique_ptr<ParticleSystem> particle_;
	const int particleNum_ = 50;
	float particleRange_ = 0.7f;

	// 点(ゴールに移動)
	Vector3 pos_{};

	// ゴール地点
	Vector3 goal_{};

	GameContext* context_ = nullptr;
};

