#include "MapTile.h"
#include "GameContext.h"
#include "Camera.h"
#include "Entity.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <memory>

void MapTile::Initialize(Entity* wall, Entity* floor,Entity* goal) {
    wall_ = wall;
    floor_ = floor;
    goal_ = goal;
}

void MapTile::LoadCSV(const std::string& filePath) {
    map_.clear();
    mapWidth_ = 0;
    mapHeight_ = 0;

    std::ifstream file(filePath);
    std::string line;

    if (!file.is_open()) {
        assert(false);
    }
    assert(file.is_open());
    
    // ファイル読む
    while (std::getline(file, line)) {
        std::vector<Tile> row;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ',')) {
            int value = std::stoi(cell);
            row.push_back(Tile(value));
        }

        if (mapWidth_ == 0) { // 一度だけ
            mapWidth_ = static_cast<int>(row.size());
        }
        map_.push_back(row);
    }
    mapHeight_ = static_cast<int>(map_.size());
    std::reverse(map_.begin(), map_.end());

    // トランスフォーム設定
    std::vector<Transform> transformsFloor;
    std::vector<Transform> transformsWall;
    std::vector<Transform> transformsGoal;
    for (int x = 0; x < mapWidth_; ++x) {
        for (int y = 0; y < mapHeight_; ++y) {
            // 床
            Transform transformFloor;
            if (map_[y][x] == Tile::Floor) {
                transformFloor.translate.x = float(x) * tileSize_ + tileSize_/2.0f;
                transformFloor.translate.y = -tileSize_;
                transformFloor.translate.z = float(y) * tileSize_ + tileSize_ / 2.0f;
                transformFloor.scale = { tileSize_,tileSize_ ,tileSize_ };
            } else {
                transformFloor.translate.y = 200.0f;
            }
            transformsFloor.push_back(transformFloor);

			// 壁
            Transform transformWall;
            if (map_[y][x] == Tile::LeftWall ||
                map_[y][x] == Tile::RightWall || 
                map_[y][x] == Tile::UpWall || 
                map_[y][x] == Tile::BottomWall) {
                transformWall.translate.x = float(x) * tileSize_ + tileSize_ / 2.0f;
                transformWall.translate.y = 0;
                transformWall.translate.z = float(y) * tileSize_ + tileSize_ / 2.0f;
                transformWall.scale = { tileSize_,tileSize_ ,tileSize_ };

                // 方向に応じて回転

            } else {
                transformWall.translate.y = 200.0f;
            }
            transformsWall.push_back(transformWall);


            // ゴール
            Transform transformGoal;
            if (map_[y][x] == Tile::Goal) {
                transformGoal.translate.x = float(x) * tileSize_ + tileSize_ / 2.0f;
                transformGoal.translate.y = 0;
                transformGoal.translate.z = float(y) * tileSize_ + tileSize_ / 2.0f;
                transformGoal.scale = { tileSize_,tileSize_ ,tileSize_ };
            } else {
                transformGoal.translate.y = 200.0f;
            }
            transformsGoal.push_back(transformGoal);
        }
    }
    floor_->SetInstanceTransforms(transformsFloor);
    wall_->SetInstanceTransforms(transformsWall);
	goal_->SetInstanceTransforms(transformsGoal);
}

void MapTile::Draw(GameContext* context,Camera* camera) {
    context->DrawEntity(*wall_, *camera);
    context->DrawEntity(*floor_, *camera);
	context->DrawEntity(*goal_, *camera);
}
