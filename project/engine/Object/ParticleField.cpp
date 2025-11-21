#include "ParticleField.h"
#include "Particle.h"

void ParticleField::Update(Particle* particle) {
	if (IsCollision(area_, particle->transform.translate)) {
		particle->transform.translate += acceleration_;
	}
}
