#pragma once

class Player;
class Enemy;
class Bullet;
class Camera;

class CollisionChecker {
public:
	void Check(Player* player, Bullet* bullet, Camera* camera);
	void Check(Enemy* enemy, Bullet* bullet, Camera* camera);
};

