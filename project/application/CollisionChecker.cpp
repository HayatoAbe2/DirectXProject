#include "CollisionChecker.h"
#include "Player.h"
#include "Enemy.h"
#include "MathUtils.h"
#include "Bullet.h"
#include "Camera.h"
#include "EffectManager.h"

void CollisionChecker::Inititalize(EffectManager* effectManager) {
	effectManager_ = effectManager;
}

void CollisionChecker::Check(Player* player, Bullet* bullet,Camera* camera) {
	// 敵の弾でなかったら判定しない
	if (!bullet->IsEnemyBullet() || bullet->IsDead()) { return; }
	
	if (Length(player->GetTransform().translate - bullet->GetTransform().translate) <=
		player->GetRadius() + bullet->GetTransform().scale.x / 2.0f) {
		player->Hit(bullet->GetDamage(), bullet->GetPrePos());
		bullet->Hit();
		camera->StartShake(1.0f, 3);
		effectManager_->SpawnHitEffect(player->GetTransform().translate);
	}
}

void CollisionChecker::Check(Enemy* enemy, Bullet* bullet, Camera* camera) {
	// 敵の弾だったら判定しない
	if (bullet->IsEnemyBullet() || bullet->IsDead()) { return; }

	if (Length(enemy->GetTransform().translate - bullet->GetTransform().translate) <=
		enemy->GetRadius() + bullet->GetTransform().scale.x / 2.0f) {
		enemy->Hit(bullet->GetDamage(),bullet->GetPrePos());
		bullet->Hit();
		camera->StartShake(1.0f, 3);
		effectManager_->SpawnHitEffect(enemy->GetTransform().translate);
	}
}
