#include "DeathParticles.h"
#include <algorithm>
#include "Graphics.h"

void DeathParticles::Initialize(ModelData* model,const Vector3 &position) {
	// NULLポインタチェック
	assert(model);
	
	// 引数として受け取ったデータを記録
	model_ = model;

	// ワールド変換の初期化
	for (Transform& worldTransform : worldTransforms_) {
		worldTransform = { {1.0f,1.0f,1.0f} };
		worldTransform.translate = position;
		worldTransform.scale = {0.5f, 0.5f, 0.5f};
	}
	color_ = {1, 1, 1, 1};
}

void DeathParticles::Update() {
	if (isFinished_) {
		// 終了していたら何もしない
		return;
	}

	for (uint32_t i = 0; i < kNumParticles; ++i) {
		// 速度
		Vector3 velocity = {kSpeed, 0, 0};
		// 回転角の計算
		float angle = kAngleUnit * i;
		// Z軸回転行列
		Matrix4x4 matrixRotation = MakeRotateZMatrix(angle);
		// 基本ベクトルを回転させて速度ベクトルを得る
		velocity = TransformVector(velocity, matrixRotation);
		// 移動
		worldTransforms_[i].translate.x += velocity.x;
		worldTransforms_[i].translate.y += velocity.y;
		worldTransforms_[i].translate.z += velocity.z;
	}
	
	// カウンターを1フレーム分進める
	counter_ += 1.0f / 60.0f;
	// 存続時間の上限に達したら
	if (counter_ >= kDuration) {
		counter_ = kDuration;
		// 終了
		isFinished_ = true;
	}

	color_.w = std::clamp((kDuration - counter_) / kDuration, 0.0f, 1.0f);

}
