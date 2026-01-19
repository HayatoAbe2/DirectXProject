#pragma once
#define NOMINMAX
#include <vector>
#include "MapTile.h"
#include "MathUtils.h"

class Player;

class MapCheck {
public:
	void Initialize(std::vector<std::vector<MapTile::Tile>> map, float tileSize);
	bool ResolveCollisionX(Vector2& pos, float radius, bool isFlying);
	bool ResolveCollisionY(Vector2& pos, float radius, bool isFlying);
	bool IsHitWall(const Vector2& pos, float radius);
	bool IsFall(const Vector2& pos, float radius);
	bool IsGoal(const Vector2& pos, float radius, bool canGoal);

private:	
	std::vector<std::vector<MapTile::Tile>> map_;
	float tileSize_ = 0;
};

