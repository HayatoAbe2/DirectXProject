#pragma once
#define NOMINMAX
#include <vector>
#include "MapTile.h"
#include "MathUtils.h"

class Player;

class MapCheck {
public:
	void Initialize(std::vector<std::vector<MapTile::Tile>> map, float tileSize);
	void ResolveCollisionX(Vector2& pos, float radius);
	void ResolveCollisionY(Vector2& pos, float radius);

private:	
	std::vector<std::vector<MapTile::Tile>> map_;
	float tileSize_ = 0;
};

