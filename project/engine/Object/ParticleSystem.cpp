#include "ParticleSystem.h"
#include "Camera.h"
#include "InstancedModel.h"
#include <numbers>

void ParticleSystem::Initialize(const std::shared_ptr<InstancedModel> model) {
	instancedModel_ = model;
	particles_.resize(model->GetNumInstance());
}

void ParticleSystem::Update() {
	for (auto& particle : particles_) {
		if (particle.alive) {
			particle.transform.translate += particle.velocity;

			// フィールドの影響
			for (const auto& field : fields_) {
				field->Update(&particle);
			}

			// 時間
			particle.lifeTime--;
			if (particle.lifeTime < 0) {
				particle.alive = false;
			}
		}
	}
}

void ParticleSystem::PreDraw(const Camera& camera) {
	std::vector<Transform> transforms;
	std::vector<Vector4> colors;

	for (auto& particle : particles_) {

		particle.transform.rotate = camera.transform_.rotate;
		transforms.push_back(particle.transform);

		particle.color.w = float(particle.lifeTime) / float(maxLifeTime_);
		colors.push_back(particle.color);
	}
	instancedModel_->UpdateInstanceTransform(camera, transforms, colors);
}

void ParticleSystem::SetColor(const Vector4& color) {
	for (auto& particle : particles_) {
		particle.color = color;
	}
}

void ParticleSystem::Emit(const Transform& baseTransform, const Vector3& velocity) {
	for (auto& particle : particles_) {
		if (!particle.alive) {
			particle.alive = true;
			particle.lifeTime = maxLifeTime_;
			particle.transform = baseTransform;
			particle.velocity = velocity;
			break;
		}
	}
}