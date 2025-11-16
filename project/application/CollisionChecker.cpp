#include "CollisionChecker.h"
#include "Player.h"
#include "MathUtils.h"

void CollisionChecker::Check(Player* player, Bullet* bullet) {
	// 敵の弾でなかったら判定しない
	if (!bullet->IsEnemyBullet()) { return; }
	
	if (Length(player->GetTransform().translate - bullet->GetTransform().translate) <=
		player->GetRadius() + bullet->GetTransform().scale.x / 2.0f) {
		player->Hit(bullet->GetDamage());
	}
}
