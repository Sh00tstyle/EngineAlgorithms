#include "OBB.h"

OBB::OBB(GameObject* pOwner, glm::vec3 pHalfSize):BoundingBox(pOwner, pHalfSize, 2) {
}

OBB::~OBB() {
}

bool OBB::collidesWith(BoundingBox * other) {
	return other->collidesWith(this); //tell other bounding box which exact type this is
}

bool OBB::collidesWith(AABB * other) {
	return isColliding(other, this); //AABB vs OBB
}

bool OBB::collidesWith(OBB * other) {
	return isColliding(this, other); //OBB vs OBB
}

bool OBB::collidesWithSwitch(BoundingBox * other) {
	switch(other->getType()) {
		case 1:
			return isColliding((AABB*)other, this); //AABB vs OBB

		case 2:
			return isColliding(this, (OBB*)other); //OBB vs OBB

		default:
			return false;
	}
}