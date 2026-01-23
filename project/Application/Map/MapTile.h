#pragma once
#include <vector>
#include <string>
#include <memory>
#include <GameContext.h>

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
	void Initialize(std::unique_ptr<InstancedModel> wall,std::unique_ptr<InstancedModel> floor,std::unique_ptr<Model> goal, GameContext* context);
	void LoadCSV(const std::string& filePath);
	void Update(bool canGoal);
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

	std::unique_ptr<InstancedModel> wall_ = nullptr;
	std::unique_ptr<InstancedModel> floor_ = nullptr;
	std::unique_ptr<Model> goal_ = nullptr;

	std::unique_ptr<ParticleSystem> particle_ = nullptr;
	const int particleNum_ = 30;
	float particleRange_ = 3.0f;
	int emitTimer_ = 0;
	const int emitTime_ = 5;
	bool soundPlayed_ = false;

	// ライトのインデックス
	int lightIndex_ = -1;

	GameContext* context_;
};

