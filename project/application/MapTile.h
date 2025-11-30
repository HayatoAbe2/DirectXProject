#pragma once
#include <vector>
#include <string>
#include <memory>

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

	~MapTile();
	void Initialize(Entity* wall,Entity* floor,Entity* goal, GameContext* context);
	void LoadCSV(const std::string& filePath);
	void Update();
	void Draw(Camera* camera);

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

	std::unique_ptr<Entity> particle_ = nullptr;
	const int particleNum_ = 30;
	float particleRange_ = 3.0f;
	int emitTimer_ = 0;
	const int emitTime_ = 5;

	// ライトのインデックス
	int lightIndex_ = 0;

	GameContext* context_;
};

