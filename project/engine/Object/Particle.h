#pragma once
#include "Transform.h"
#include "MathUtils.h"

struct Particle {
    Transform transform;
    Vector3 velocity;
    int lifetime;
    bool alive = false;
};