#pragma once

class Player;
class Enemy;
class Bullet;

class CollisionChecker {
public:
	void Check(Player* player, Bullet* bullet);
	void Check(Enemy* enemy, Bullet* bullet);
};

