#include "ParticleField.h"
#include "Particle.h"

void ParticleField::Update(Particle* particle) {
	if (!useArea_ || (useArea_ && IsCollision(area_, particle->transform.translate))) {
		particle->transform.translate += acceleration_;

		// 重力
		if (gravity_ != 0) {
			Vector3 toCenter = Normalize(center_ - particle->transform.translate);
			particle->transform.translate += toCenter * gravity_;
			particle->transform.rotate = { -sinf(toCenter.x),cosf(toCenter.y),0.0f };
		}

		// 回転
		if (rotateXZ_ != 0) {

			float dtheta = rotateXZ_;
			float cosT = cosf(dtheta);
			float sinT = sinf(dtheta);

			// 中心からの相対ベクトル
			float dx = particle->transform.translate.x - rotateCenter_.x;
			float dz = particle->transform.translate.z - rotateCenter_.z;

			// Δθ分だけ回転
			float rx = dx * cosT - dz * sinT;
			float rz = dx * sinT + dz * cosT;

			// 戻す
			particle->transform.translate.x = rotateCenter_.x + rx;
			particle->transform.translate.z = rotateCenter_.z + rz;
		}

	}
}
