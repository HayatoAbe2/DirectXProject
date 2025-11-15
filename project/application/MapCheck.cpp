#include "MapCheck.h"
#include "MapTile.h"
#include "Player.h"
#include <algorithm>

void MapCheck::Initialize(std::vector<std::vector<MapTile::Tile>> map, float tileSize) {
    map_ = map;
    tileSize_ = tileSize;
}

void MapCheck::ResolveCollisionX(Vector2& pos, float radius) {
    int mapH = static_cast<int>(map_.size());
    int mapW = static_cast<int>(map_[0].size());

    // キャラのAABB（更新後）
    float charMinX = pos.x - radius;
    float charMaxX = pos.x + radius;
    float charMinY = pos.y - radius;
    float charMaxY = pos.y + radius;

    // 衝突しそうな範囲だけループ（効率化）
    int startY = std::max(0, static_cast<int>(charMinY / tileSize_));
    int endY = std::min(mapH - 1, static_cast<int>(charMaxY / tileSize_));
    int startX = std::max(0, static_cast<int>(charMinX / tileSize_));
    int endX = std::min(mapW - 1, static_cast<int>(charMaxX / tileSize_));

    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            if (map_[y][x] == MapTile::Tile::Floor) continue;

            // タイルAABB
            float tileMinX = x * tileSize_;
            float tileMinY = y * tileSize_;
            float tileMaxX = tileMinX + tileSize_;
            float tileMaxY = tileMinY + tileSize_;

            // Y方向が重なってなければ無視（X軸解決なので）
            if (charMaxY <= tileMinY || charMinY >= tileMaxY) continue;

            // X方向重なり
            float overlapX = std::min(charMaxX, tileMaxX) - std::max(charMinX, tileMinX);
            if (overlapX > 0.0f) {
                float tileCenterX = (tileMinX + tileMaxX) * 0.5f;

                if (pos.x > tileCenterX)
                    pos.x += (tileMaxX - charMinX); // 壁の右側にいる → 右に押す
                else
                    pos.x -= (charMaxX - tileMinX); // 壁の左側にいる → 左に押す

                // 押し戻したのでAABB更新
                charMinX = pos.x - radius;
                charMaxX = pos.x + radius;
            }
        }
    }

    // --- マップ外チェック ---
    float minX = 0;
    float maxX = mapW * tileSize_;
    pos.x = std::clamp(pos.x, minX + radius, maxX - radius);
}

void MapCheck::ResolveCollisionY(Vector2& pos, float radius) {
    int mapH = static_cast<int>(map_.size());
    int mapW = static_cast<int>(map_[0].size());

    float charMinX = pos.x - radius;
    float charMaxX = pos.x + radius;
    float charMinY = pos.y - radius;
    float charMaxY = pos.y + radius;

    int startY = std::max(0, static_cast<int>(charMinY / tileSize_));
    int endY = std::min(mapH - 1, static_cast<int>(charMaxY / tileSize_));
    int startX = std::max(0, static_cast<int>(charMinX / tileSize_));
    int endX = std::min(mapW - 1, static_cast<int>(charMaxX / tileSize_));

    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            if (map_[y][x] == MapTile::Tile::Floor) continue;

            float tileMinX = x * tileSize_;
            float tileMinY = y * tileSize_;
            float tileMaxX = tileMinX + tileSize_;
            float tileMaxY = tileMinY + tileSize_;

            // X方向が重なっていなければ無視
            if (charMaxX <= tileMinX || charMinX >= tileMaxX) continue;

            // Y方向重なり
            float overlapY = std::min(charMaxY, tileMaxY) - std::max(charMinY, tileMinY);
            if (overlapY > 0.0f) {
                float tileCenterY = (tileMinY + tileMaxY) * 0.5f;

                // ⬇️ 押し戻し方向を修正（上下が逆にならないように）
                if (pos.y > tileCenterY)
                    pos.y += (tileMaxY - charMinY); // キャラが下側 → 下に押し戻す（正方向）
                else
                    pos.y -= (charMaxY - tileMinY); // キャラが上側 → 上に押し戻す（負方向）

                // AABB更新
                charMinY = pos.y - radius;
                charMaxY = pos.y + radius;
            }
        }
    }

    // マップ外チェック
    float minY = 0;
    float maxY = mapH * tileSize_;
    pos.y = std::clamp(pos.y, minY + radius, maxY - radius);
}