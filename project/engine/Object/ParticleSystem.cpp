#include "ParticleSystem.h"
#include "Camera.h"
#include "InstancedModel.h"

void ParticleSystem::Initialize(const std::shared_ptr<InstancedModel> model) {
	instancedModel_ = model;
	particles_.resize(model->GetNumInstance());
}

void ParticleSystem::Update() {
	for (Particle& particle : particles_) {
		if (particle.alive) {
			particle.transform.translate += particle.velocity;

			// フィールドの影響
			for (const std::unique_ptr<ParticleField>& field : fields_) {
				field->Update(&particle);
			}

			if (fadeout_) {
			}

			// 時間
			particle.lifetime--;
			if (particle.lifetime < 0) {
				particle.alive = false;
			}

		}
	}
}

void ParticleSystem::PreDraw(const Camera& camera) {
	std::vector<Transform> transforms;

	for (Particle& particle : particles_) {
		if (particle.alive) {
			particle.transform.rotate = -camera.transform_.rotate;
			transforms.push_back(particle.transform);
		}
	}
	instancedModel_->UpdateInstanceTransform(camera, transforms);
}

void ParticleSystem::Emit(const Transform& baseTransform, const Vector3& velocity) {
	for (Particle& particle : particles_) {
		if (!particle.alive) {
			particle.alive = true;
			particle.lifetime = lifeTime_;
			particle.transform = baseTransform;
			particle.velocity = velocity;
			break;
		}
	}
}