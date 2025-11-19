#pragma once
#include <vector>
#include <string>

class Entity;
class GameContext;
class Camera;

class MapTile {
public:
	enum Tile {
		None,
		Floor,
		LeftWall,
		RightWall,
		UpWall,
		BottomWall,
		Goal,
	};

	void Initialize(Entity* wall,Entity* floor,Entity* goal);
	void LoadCSV(const std::string& filePath);

	void Draw(GameContext* context, Camera* camera);

	std::vector<std::vector<Tile>> GetMap() { return map_; }
	int GetWidth() { return mapWidth_; }
	int GetHeight() { return mapHeight_; }
	float GetTileSize() { return tileSize_; }

private:
	std::vector<std::vector<Tile>> map_;
	float tileSize_ = 1.5f;
	int mapWidth_ = 0;
	int mapHeight_ = 0;

	Entity* wall_ = nullptr;
	Entity* floor_ = nullptr;
	Entity* goal_ = nullptr;
};

