#include "OBB.h"

OBB::OBB(GameObject* pOwner, glm::vec3 pHalfSize):BoundingBox(pOwner, pHalfSize) {
}

OBB::~OBB() {
}