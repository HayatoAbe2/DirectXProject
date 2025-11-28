#include "ParticleField.h"
#include "Particle.h"

void ParticleField::Update(Particle* particle) {
	if (!useArea_ || (useArea_ && IsCollision(area_, particle->transform.translate))) {
		particle->transform.translate += acceleration_;

		if (gravity_ != 0) {
			Vector3 toCenter = Normalize(center_ - particle->transform.translate);
			particle->transform.translate += toCenter * gravity_;
			particle->transform.rotate = { -sinf(toCenter.x),cosf(toCenter.y),0.0f };
		}
	}
}
