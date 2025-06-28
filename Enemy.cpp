#include "Enemy.h"
#include "numbers"
#include <cmath>

void Enemy::Initialize(Model* model, const Vector3& position) {
	// NULLポインタチェック
	assert(model);
	// 引数として受け取ったデータを記録
	model_ = model;
	// ワールドトランスフォームの初期化
	worldTransform_ = { {1.0f,1.0f,1.0f} };
	worldTransform_.translate = position;
	worldTransform_.rotate.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	worldTransform_.scale = {0.5f, 0.5f, 0.5f};
	// 速度を設定
	velocity_ = {-kWalkSpeed, 0, 0};
	walkTimer_ = 0.0f;
}

void Enemy::Update() {
	// タイマー加算
	walkTimer_ += 1.0f / 30.0f;
	// 回転アニメーション
	float param = std::sin(walkTimer_);
	float degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	worldTransform_.rotate.x = degree * float(std::numbers::pi) / 180.0f;

	// 移動
	worldTransform_.translate.x += velocity_.x;

	aabb_ = {
	    {worldTransform_.translate.x - kBlank, worldTransform_.translate.y - kBlank, worldTransform_.translate.z - kBlank},
	    {worldTransform_.translate.x + kBlank, worldTransform_.translate.y + kBlank, worldTransform_.translate.z + kBlank}
    };

}

void Enemy::OnCollision(const Player* player) { 
	(void)player; 
}

void Enemy::Draw(Camera& camera, Graphics& graphics) {
	model_->SetTransform(worldTransform_);
	model_->UpdateTransformation(camera);
	model_->Draw(graphics);
}