#include "Entity.h"
#include "ParticleSystem.h"

uint32_t Entity::nextId_ = 0;

Entity::Entity() {
	// ID割り当て&インクリメント
	id_ = nextId_++;
}

Entity::~Entity() {
	particleSystem_.reset();
}


void Entity::SetParticleSystem(const std::shared_ptr<InstancedModel>& m) {
	particleSystem_ = std::make_shared<ParticleSystem>();
	particleSystem_->Initialize(m);
}

bool Entity::IsRenderable() {
	return model_ || sprite_ || instancedModel_ || particleSystem_;
	//|| spriteInstance_;
}
