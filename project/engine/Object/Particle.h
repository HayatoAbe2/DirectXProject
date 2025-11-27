#pragma once
#include "Transform.h"
#include "MathUtils.h"

struct Particle {
    Transform transform;
    Vector3 velocity;
    Vector4 color;
    int lifeTime;
    bool alive = false;
};