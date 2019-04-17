#include "AABB.h"

AABB::AABB(GameObject* pOwner, glm::vec3 pHalfSize) :BoundingBox(pOwner, pHalfSize, 1) {
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

bool AABB::collidesWithSwitch(BoundingBox * other) {
	switch(other->getType()) {
		case 1:
			return isColliding(this, (AABB*)other); //AABB vs AABB

		case 2:
			return isColliding(this, (OBB*)other); //AABB vs OBB

		default:
			return false;
	}
}