#include "AABB.h"

AABB::AABB(GameObject* pOwner, glm::vec3 pHalfSize) :BoundingBox(pOwner, pHalfSize) {
}

AABB::~AABB() {
}

bool AABB::collidesWith(BoundingBox * other) {
	return other->collidesWith(this); //tell other bounding box which exact type this is
}

bool AABB::collidesWith(AABB * other) {
	return isColliding(this, other); //AABB vs AABB
}

bool AABB::collidesWith(OBB * other) {
	return isColliding(this, other); //AABB vs OBB
}