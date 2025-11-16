#include "FireBall.h"
#include "Bullet.h"
#include "GameContext.h"
#include "Entity.h"

int FireBall::Shoot(Vector3 pos, Vector3 dir,std::vector<std::unique_ptr<Bullet>>& bullets,GameContext* context) {
		auto bulletModel = std::make_unique<Entity>();
		bulletModel->SetTranslate(pos);
		bulletModel->SetModel(context->LoadModel(status_.bulletDirectoryPath, status_.bulletFileName));
		std::unique_ptr newBullet = std::make_unique<Bullet>(std::move(bulletModel), dir * status_.bulletSpeed, status_);
		bullets.push_back(std::move(newBullet));
		return status_.shootCoolTime;
}

void FireBall::Update() {
}

