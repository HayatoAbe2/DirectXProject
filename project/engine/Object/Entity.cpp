#include "Entity.h"
uint32_t Entity::nextId_ = 0;

Entity::Entity(std::shared_ptr<Sprite> sprite) {
	sprite_ = sprite;

	// ID割り当て&インクリメント
	id_ = nextId_++;
}
Entity::Entity(std::shared_ptr<Model> model) {
	model_ = model;

	// ID割り当て&インクリメント
	id_ = nextId_++;
}

bool Entity::IsRenderable() {
	return model_ || sprite_;
	//|| modelInstance_ || spriteInstance_;
}
