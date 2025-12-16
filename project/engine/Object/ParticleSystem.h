#pragma once
#include "Particle.h"
#include "ParticleField.h"
#include <vector>
#include <memory>

class InstancedModel;
class Camera;

class ParticleSystem {
public:
    void Initialize(std::unique_ptr<InstancedModel> model);
    void Emit(const Transform& baseTransform, const Vector3& velocity);
    void Update();
    void PreDraw(Camera* camera);

    void SetColor(const Vector4& color);
    void SetLifeTime(int lifeTime) { maxLifeTime_ = lifeTime; }
    InstancedModel* GetInstancedModel() const { return instancedModel_.get(); }

	void AddField(std::unique_ptr<ParticleField> field) {
		fields_.push_back(std::move(field));
	}

    void RemoveField() {
        fields_.clear();
    }

private:
    std::vector<Particle> particles_;
    std::unique_ptr<InstancedModel> instancedModel_;
	std::vector<std::unique_ptr<ParticleField>> fields_;
    int maxLifeTime_ = 1;
};
