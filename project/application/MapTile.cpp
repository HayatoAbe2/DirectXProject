#include "MapTile.h"
#include "GameContext.h"
#include "Camera.h"
#include "Entity.h"
#include "ParticleSystem.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <memory>

void MapTile::Initialize(Entity* wall, Entity* floor,Entity* goal,GameContext* context) {
    wall_ = wall;
    floor_ = floor;
    goal_ = goal;

    particle_ = std::make_unique<Entity>();
    particle_->SetParticleSystem(context->LoadInstancedModel("Resources/Particle/Goal", "goalEffect.obj", particleNum_));
    particle_->GetParticleSystem()->SetLifeTime(25);
    particle_->GetParticleSystem()->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });
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
    Transform transformGoal;
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
            if (map_[y][x] == Tile::Goal) {
                transformGoal.translate.x = float(x) * tileSize_ + tileSize_ / 2.0f;
                transformGoal.translate.y = 0;
                transformGoal.translate.z = float(y) * tileSize_ + tileSize_ / 2.0f;
                transformGoal.scale = { tileSize_,tileSize_ ,tileSize_ };
            }
        }
    }
    floor_->SetInstanceTransforms(transformsFloor);
    wall_->SetInstanceTransforms(transformsWall);
	goal_->SetTransform(transformGoal);
}

void MapTile::Update(GameContext* context) {
    emitTimer_++;
    if (emitTimer_ >= emitTime_) {
        // パーティクル
        for (int i = 0; i < 1; ++i) {
            Vector3 randomVector = {
            context->RandomFloat(-particleRange_ / 2.0f, particleRange_ / 2.0f),
            context->RandomFloat(-particleRange_ / 2.0f, particleRange_ / 2.0f),
            context->RandomFloat(-particleRange_ / 2.0f, particleRange_ / 2.0f),
            };
            Transform transform = goal_->GetTransform();
            transform.translate += randomVector;
            transform.scale = { 0.5f,1.2f,1.2f };
            particle_->GetParticleSystem()->Emit(transform, { 0,0.5f,0 });
        }
        emitTimer_ = 0;
    }
    particle_->GetParticleSystem()->Update();
}

void MapTile::Draw(GameContext* context,Camera* camera) {
    context->DrawEntity(*wall_, *camera);
    context->DrawEntity(*floor_, *camera);
	context->DrawEntity(*goal_, *camera);
	context->DrawEntity(*particle_, *camera,BlendMode::Add);
}
