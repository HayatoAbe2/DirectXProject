#pragma once
#include "Particle.h"
#include "ParticleField.h"
#include <vector>
#include <memory>

class InstancedModel;
class Camera;

class ParticleSystem {
public:
    void Initialize(const std::shared_ptr<InstancedModel> model);
    void Emit(const Transform& baseTransform, const Vector3& velocity);
    void Update();
    void PreDraw(const Camera& camera);

    void SetLifeTime(int lifeTime) { lifeTime_ = lifeTime; }
    std::shared_ptr<InstancedModel> GetInstancedModel_() const { return instancedModel_; };

	void AddField(std::unique_ptr<ParticleField> field) {
		fields_.push_back(std::move(field));
	}

private:
    std::vector<Particle> particles_;
    std::shared_ptr<InstancedModel> instancedModel_;
	std::vector<std::unique_ptr<ParticleField>> fields_;
    int lifeTime_ = 1;
    bool fadeout_ = true;
};
