#pragma once
#include "MathUtils.h"
#include "Particle.h"

class ParticleField {
public:
	void Update(Particle* particle);

	void SetAcceleration(const Vector3& acceleration) { acceleration_ = acceleration; }
	void SetGravity(float gravity, const Vector3& center) { gravity_ = gravity; center_ = center; }
	
	void SetArea(const AABB& area) { area_ = area; }
	void SetCheckArea(bool useArea) { useArea_ = useArea; }

	void SetRotateXZ(float rotateSpeed, const Vector3& center) { rotateXZ_ = rotateSpeed; rotateCenter_ = center;
	}
private:
	Vector3 acceleration_{};

	// 重力の中心点
	Vector3 center_{};
	float gravity_ = 0;

	AABB area_{};
	bool useArea_ = true;

	// 回転の中心点
	Vector3 rotateCenter_{};
	float theta_ = 0;
	float rotateXZ_ = 0;
	float rotateRadius_ = 0;

};

