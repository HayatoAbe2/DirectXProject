#include "FireBall.h"
#include "Bullet.h"
#include "GameContext.h"
#include "Entity.h"

int FireBall::Shoot(Vector3 pos, Vector3 dir,std::vector<std::unique_ptr<Bullet>>& bullets,GameContext* context) {
		auto bulletEntity = std::make_unique<Entity>();
		bulletEntity->SetTranslate(pos);
		bulletEntity->SetModel(status_.bulletModel);
		std::unique_ptr newBullet = std::make_unique<Bullet>(std::move(bulletEntity), dir * status_.bulletSpeed, status_);
		bullets.push_back(std::move(newBullet));
		return status_.shootCoolTime;
}

void FireBall::Update() {
}

