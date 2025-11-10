#include "PlayerBullet.h"
#include "GameContext.h"
#include "Entity.h"
#include "Camera.h"
#include "App.h"

PlayerBullet::PlayerBullet(Entity* entity,const Vector3& velocity) {
	model_ = entity;
	velocity_ = velocity;
}

void PlayerBullet::Update() {
	transform_.translate += velocity_;
}

void PlayerBullet::Draw(Camera& camera) {
}
