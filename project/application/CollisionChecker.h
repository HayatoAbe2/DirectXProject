#pragma once

class Player;
class Enemy;
class Bullet;

class CollisionChecker {
	void Check(Player* player, Bullet* bullet);
};

