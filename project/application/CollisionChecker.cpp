#include "CollisionChecker.h"
#include "Player.h"
#include "Enemy.h"
#include "MathUtils.h"
#include "Bullet.h"

void CollisionChecker::Check(Player* player, Bullet* bullet) {
	// 敵の弾でなかったら判定しない
	if (!bullet->IsEnemyBullet()) { return; }
	
	if (Length(player->GetTransform().translate - bullet->GetTransform().translate) <=
		player->GetRadius() + bullet->GetTransform().scale.x / 2.0f) {
		player->Hit(bullet->GetDamage(),bullet->GetTransform().translate);
		bullet->Hit();
	}
}

void CollisionChecker::Check(Enemy* enemy, Bullet* bullet) {
	// 敵の弾だったら判定しない
	if (bullet->IsEnemyBullet()) { return; }

	if (Length(enemy->GetTransform().translate - bullet->GetTransform().translate) <=
		enemy->GetRadius() + bullet->GetTransform().scale.x / 2.0f) {
		enemy->Hit(bullet->GetDamage(),bullet->GetTransform().translate);
		bullet->Hit();
	}
}
