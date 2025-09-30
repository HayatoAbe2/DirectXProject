#include "Sphere.h"

bool Sphere::CheckCollision(const Sphere sphere1, const Sphere sphere2) {
	if (Length(sphere2.center - sphere1.center) <= sphere1.radius + sphere2.radius) {
		return true;
	}
	return false;
}
