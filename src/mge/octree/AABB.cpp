#include "AABB.h"

AABB::AABB(GameObject* pOwner, glm::vec3 pHalfSize) :BoundingBox(pOwner, pHalfSize) {
}

AABB::~AABB() {
}