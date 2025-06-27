#define NOMINMAX
#include "Player.h"
#include "MapChipField.h"
#include "Transform.h"
#include <algorithm>
#include <cassert>
#include <numbers>
#include "math.h"
#include "cmath"
#include "array"
#include "Camera.h"
#include "Graphics.h"

void Player::Initialize(Model* model, const Vector3& position,Input* input) {
	// NULLポインタチェック
	assert(model);

	// 引数として受け取ったデータを記録
	model_ = model;
	input_ = input;

	// ワールドトランスフォームの初期化
	worldTransform_ = { {1.0f,1.0f,1.0f} };
	worldTransform_.translate = position;
	worldTransform_.rotate.y = std::numbers::pi_v<float> / 2.0f;
	worldTransform_.scale = {0.5f, 0.5f, 0.5f};
}

void Player::Update() {
	// 移動入力
	InputMove();
	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.velocity = velocity_;
	// マップ衝突チェック
	CheckMapCollision(collisionMapInfo);
	// 接地/空中状態の切り替え
	SwitchLanding(collisionMapInfo);
	// 衝突した各方向に対する処理
	OnHitTop(collisionMapInfo);
	OnHitBottom(collisionMapInfo);
	OnHitWall(collisionMapInfo);
	// 移動
	Move(collisionMapInfo);

	// 旋回制御
	{
		if (turnTimer_ > 0.0f) {
			turnTimer_ -= 1.0f / 60.0f;

			// 左右の自キャラ角度テーブル
			float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
			// 状態に応じた角度を取得する
			float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
			// 自キャラの角度を設定する
			float t = 1.0f - (turnTimer_ / kTimeTurn);
			if (t < 0.5f) {
				float easedT = 2.0f * t * t;
				worldTransform_.rotate.y = turnFirstRotationY_ + (destinationRotationY - turnFirstRotationY_) * easedT;
			} else {
				float easedT = 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
				worldTransform_.rotate.y = turnFirstRotationY_ + (destinationRotationY - turnFirstRotationY_) * easedT;
			}
		}

		// aabb更新
		aabb_ = {
		    {worldTransform_.translate.x - kBlank, worldTransform_.translate.y - kBlank, worldTransform_.translate.z - kBlank},
		    {worldTransform_.translate.x + kBlank, worldTransform_.translate.y + kBlank, worldTransform_.translate.z + kBlank}
        };
	}
}

void Player::InputMove() {
	// 移動入力
	if (onGround_) {

		if (input_->IsPress(DIK_RIGHT) || input_->IsPress(DIK_LEFT)) {
			Vector3 acceleration = {};

			if (input_->IsPress(DIK_RIGHT)) {
				// 左移動中の右入力
				if (velocity_.x < 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x += kAcceleration;

				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotate.y;
					turnTimer_ = kTimeTurn;
				}
			} else if (input_->IsPress(DIK_LEFT)) {
				// 右移動中の左入力
				if (velocity_.x > 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x -= kAcceleration;

				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotate.y;
					turnTimer_ = kTimeTurn;
				}
			}
			// 加速/減速
			velocity_.x += acceleration.x;

			// 最大速度
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

		} else {
			// 非入力時は移動減衰をかける
			velocity_.x *= (1.0f - kAttenuation);
		}

		if (input_->IsPress(DIK_UP) && onGround_) {
			// ジャンプ初速
			velocity_.y += kJumpAcceleration;
		}

	} else {
		// 落下速度
		velocity_.y += -kGravityAcceleration;
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}

void Player::CheckMapCollision(CollisionMapInfo& info) {
	CheckHitTop(info);
	CheckHitBottom(info);
	CheckHitLeft(info);
	CheckHitRight(info);
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0.0f}, // 右下
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0.0f}, // 左下
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0.0f}, // 右上
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0.0f}, // 左上
	};

	return {center.x + offsetTable[static_cast<uint32_t>(corner)].x, center.y + offsetTable[static_cast<uint32_t>(corner)].y, center.z + offsetTable[static_cast<uint32_t>(corner)].z};
}

void Player::CheckHitTop(CollisionMapInfo& info) {

	// 上昇中以外は判定しない
	if (info.velocity.y <= 0) {
		return;
	}

	// 移動後の4つの角の座標の計算
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(
		    {worldTransform_.translate.x + info.velocity.x, worldTransform_.translate.y + info.velocity.y, worldTransform_.translate.z + info.velocity.z}, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// プレイヤーの上方向の当たり判定
	bool hit = false;
	// 左上
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 右上
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translate);
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			float vel = rect.bottom - worldTransform_.translate.y - (kBlank + kCollisionOffset); // めり込まない移動量
			info.velocity.y = std::max(0.0f, vel);
			// 天井に当たったことを記録
			info.isHitCeiling = true;
		}
	}
}

void Player::CheckHitBottom(CollisionMapInfo& info) {
	// 下降中以外は判定しない
	if (info.velocity.y >= 0) {
		return;
	}

	// 移動後の4つの角の座標の計算
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(
		    {worldTransform_.translate.x + info.velocity.x, worldTransform_.translate.y + info.velocity.y, worldTransform_.translate.z + info.velocity.z}, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// プレイヤーの下方向の当たり判定
	bool hit = false;
	// 左下
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 右下
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translate);
		if (indexSetNow.yIndex != indexSet.yIndex) {

			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			float vel = rect.top - worldTransform_.translate.y + (kBlank + kCollisionOffset); // めり込まない移動量
			info.velocity.y = std::min(0.0f, vel);
			// 地面に当たったことを記録
			info.isLanding = true;
		}
	}
}

void Player::CheckHitLeft(CollisionMapInfo& info) {
	// 左移動中以外は判定しない
	if (info.velocity.x >= 0) {
		return;
	}

	// 移動後の4つの角の座標の計算
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(
		    {worldTransform_.translate.x + info.velocity.x, worldTransform_.translate.y + info.velocity.y, worldTransform_.translate.z + info.velocity.z}, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// プレイヤーの左方向の当たり判定
	bool hit = false;
	// 左上
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 左下
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translate);
		if (indexSetNow.xIndex != indexSet.xIndex) {

			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			float vel = rect.right - worldTransform_.translate.x + (kBlank + kCollisionOffset); // めり込まない移動量
			info.velocity.x = std::min(0.0f, vel);
			info.isHitWall = true;
		}
	}
}

void Player::CheckHitRight(CollisionMapInfo& info) {
	// 右移動中以外は判定しない
	if (info.velocity.x <= 0) {
		return;
	}

	// 移動後の4つの角の座標の計算
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(
		    {worldTransform_.translate.x + info.velocity.x, worldTransform_.translate.y + info.velocity.y, worldTransform_.translate.z + info.velocity.z}, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// プレイヤーの右方向の当たり判定
	bool hit = false;
	// 右上
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 右下
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translate);
		if (indexSetNow.xIndex != indexSet.xIndex) {

			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			float vel = rect.left - worldTransform_.translate.x - (kBlank + kCollisionOffset); // めり込まない移動量
			info.velocity.x = std::max(0.0f, vel);
			info.isHitWall = true;
		}
	}
}

void Player::SwitchLanding(CollisionMapInfo& info) {
	// 接地状態の切り替え
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			// 落下の当たり判定
			bool hit = false;
			// 移動後の4つの角の座標の計算
			std::array<Vector3, kNumCorner> positionsNew;
			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPosition(
				    {worldTransform_.translate.x + info.velocity.x, worldTransform_.translate.y + info.velocity.y, worldTransform_.translate.z + info.velocity.z}, static_cast<Corner>(i));
			}
			MapChipType mapChipType;
			// 左下
			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition({positionsNew[kLeftBottom].x, positionsNew[kLeftBottom].y - kCollisionOffset, positionsNew[kLeftBottom].z});
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}
			// 右下
			indexSet = mapChipField_->GetMapChipIndexSetByPosition({positionsNew[kRightBottom].x, positionsNew[kRightBottom].y - kCollisionOffset, positionsNew[kRightBottom].z});
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			// 落下開始
			if (!hit) {
				// 空中状態に切り替える
				onGround_ = false;
			}
		}

	} else {
		// 空中状態の処理
		if (info.isLanding) {
			// 接地状態に移行
			onGround_ = true;
			// 着地時にX速度を減衰
			velocity_.x *= (1.0f - kAttenuationLanding);
			// Y速度を0にする
			velocity_.y = 0.0f;
		}
	}
}

void Player::Move(const CollisionMapInfo& info) {
	worldTransform_.translate.x += info.velocity.x;
	worldTransform_.translate.y += info.velocity.y;
}

void Player::OnHitTop(const CollisionMapInfo& info) {
	// 天井に当たったら
	if (info.isHitCeiling) {
		velocity_.y = 0.0f;
	}
}

void Player::OnHitBottom(const CollisionMapInfo& info) {
	// ジャンプ開始
	if (velocity_.y > 0.0f) {
		onGround_ = false;
	} else {
		// 落下の当たり判定
		bool hit = false;
		// 移動後の4つの角の座標の計算
		std::array<Vector3, kNumCorner> positionsNew;
		for (uint32_t i = 0; i < positionsNew.size(); ++i) {
			positionsNew[i] = CornerPosition(
			    {worldTransform_.translate.x + info.velocity.x, worldTransform_.translate.y + info.velocity.y, worldTransform_.translate.z + info.velocity.z}, static_cast<Corner>(i));
		}
		MapChipType mapChipType;
		// 左下
		MapChipField::IndexSet indexSet;
		indexSet = mapChipField_->GetMapChipIndexSetByPosition({positionsNew[kLeftBottom].x, positionsNew[kLeftBottom].y - kCollisionOffset, positionsNew[kLeftBottom].z});
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		if (mapChipType == MapChipType::kBlock) {
			hit = true;
		}
		// 右下
		indexSet = mapChipField_->GetMapChipIndexSetByPosition({positionsNew[kRightBottom].x, positionsNew[kRightBottom].y - kCollisionOffset, positionsNew[kRightBottom].z});
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
		if (mapChipType == MapChipType::kBlock) {
			hit = true;
		}

		// 落下開始
		if (!hit) {
			// 空中状態に切り替える
			onGround_ = false;
		}
	}
}

void Player::OnHitWall(const CollisionMapInfo& info) {
	// 壁接触による減速
	if (info.isHitWall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}

void Player::OnCollision(const Enemy* enemy) {
	(void)enemy; // 今は使わない
	SetIsDead(true);
}

void Player::Draw(Camera& camera, Graphics& graphics) {
	model_->SetTransform(worldTransform_);
	model_->UpdateModel(camera);
	model_->Draw(graphics);

}