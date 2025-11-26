#include "NormalBullet.h"
#include "GameContext.h"
#include "MapCheck.h"

void NormalBullet::Update(MapCheck* mapCheck) {
	model_->SetTranslate(model_->GetTransform().translate + velocity_);

	// マップ当たり判定
	Vector2 pos = { model_->GetTransform().translate.x,model_->GetTransform().translate.z };

	lifeTime_--;
	if (lifeTime_ <= 0 || mapCheck->IsHitWall(pos, status_.bulletSize / 2.0f)) {
		Hit();
	}
}

void NormalBullet::Draw(GameContext* context, Camera* camera) {
	context->DrawEntity(*model_, *camera);
}

void NormalBullet::Hit() {
	isDead_ = true;
}