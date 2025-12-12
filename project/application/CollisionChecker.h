#pragma once

class Player;
class Enemy;
class Bullet;
class Camera;
class EffectManager;
class GameContext;

class CollisionChecker {
public:
	void Inititalize(EffectManager* effectManager, GameContext* context);
	void Check(Player* player, Bullet* bullet, Camera* camera);
	void Check(Enemy* enemy, Bullet* bullet, Camera* camera);

private:
	EffectManager* effectManager_;
	GameContext* context_;
};

