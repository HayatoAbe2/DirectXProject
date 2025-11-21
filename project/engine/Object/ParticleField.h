#pragma once
#include "MathUtils.h"
#include "Particle.h"

class ParticleField {
public:
	void Update(Particle* particle);

	void SetAcceleration(const Vector3& acceleration) { acceleration_ = acceleration; }
	void SetArea(const AABB& area) { area_ = area; }
private:
	Vector3 acceleration_{};
	AABB area_{};
};

