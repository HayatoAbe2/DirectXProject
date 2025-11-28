#pragma once
#include "MathUtils.h"
#include "Particle.h"

class ParticleField {
public:
	void Update(Particle* particle);

	void SetAcceleration(const Vector3& acceleration) { acceleration_ = acceleration; }
	void SetGravity(float gravity, const Vector3& center) { gravity_ = gravity; center_ = center; }
	
	void SetArea(const AABB& area) { area_ = area; }
	void SetUseArea(bool useArea) { useArea_ = useArea; }
private:
	Vector3 acceleration_{};
	float gravity_ = 0;

	AABB area_{};
	bool useArea_ = true;

	Vector3 center_{};
};

