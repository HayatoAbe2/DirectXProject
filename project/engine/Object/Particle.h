#pragma once
#include "Transform.h"
#include "MathUtils.h"

struct Particle {
    Transform transform;
    Vector3 velocity;
    float lifetime;
    bool alive = true;
};