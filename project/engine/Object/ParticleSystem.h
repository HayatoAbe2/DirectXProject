#pragma once
#include "Particle.h"
#include <vector>
#include <memory>

class InstancedModel;
class Camera;

class ParticleSystem {
public:
    void Initialize(const std::shared_ptr<InstancedModel> model);
    void Emit(const Transform& baseTransform, const Vector3& velocity);
    void Update(float deltaTime);
    void PreDraw(const Camera& camera);

    void SetLifeTime(float lifeTime) { lifeTime_ = lifeTime; }
    std::shared_ptr<InstancedModel> GetInstancedModel_() const { return instancedModel_; };

private:
    std::vector<Particle> particles_;
    std::shared_ptr<InstancedModel> instancedModel_;
    float lifeTime_ = 1.0f;
};
