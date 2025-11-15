#include "MapTile.h"
#include "GameContext.h"
#include "Camera.h"
#include "Entity.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <memory>

void MapTile::Initialize(Entity* wall, Entity* floor) {
    wall_ = wall;
    floor_ = floor;
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
    std::vector<Transform> transforms;
    for (int x = 0; x < mapWidth_; ++x) {
        for (int y = 0; y < mapHeight_; ++y) {
            Transform transform;
            if (map_[y][x] == Tile::Floor) {
                transform.translate.x = float(x) * tileSize_ + tileSize_/2.0f;
                transform.translate.y = -tileSize_;
                transform.translate.z = float(y) * tileSize_ + tileSize_ / 2.0f;
                transform.scale = { tileSize_,tileSize_ ,tileSize_ };
            } else {
                transform.translate.y = 200.0f;
            }
            transforms.push_back(transform);
        }
    }
    floor_->SetInstanceTransforms(transforms);
    wall_->SetInstanceTransforms(transforms);
    
}

void MapTile::Draw(GameContext* context,Camera* camera) {
    context->DrawEntity(*wall_, *camera);
    context->DrawEntity(*floor_, *camera);
}
