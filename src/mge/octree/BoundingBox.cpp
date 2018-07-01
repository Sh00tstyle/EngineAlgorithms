#include "BoundingBox.h"
#include "mge\core\GameObject.hpp"

BoundingBox::BoundingBox(glm::vec3 pCenter, glm::vec3 pHalfSize) {
	//stationary bounds for the octree
	_owner = nullptr;
	_center = pCenter;
	_halfSize = pHalfSize;
}

BoundingBox::BoundingBox(GameObject* pOwner, glm::vec3 pHalfSize) {
	//dynamic bounds for object that might move (but can be set to static)
	_owner = pOwner;
	_center = _owner->getWorldPosition();
	_halfSize = pHalfSize;
}

BoundingBox::~BoundingBox() {
}

glm::vec3 BoundingBox::getHalfSize() {
	return _halfSize;
}

glm::vec3 BoundingBox::getCenter() {
	//only update the center if we have an owner and it is not static
	if(_owner != nullptr && !_owner->IsStatic()) _center = _owner->getWorldPosition();

	return _center;
}

glm::vec3 BoundingBox::getMin() {
	return getCenter() - getHalfSize();
}

glm::vec3 BoundingBox::getMax() {
	return getCenter() + getHalfSize();
}

GameObject* BoundingBox::getOwner() {
	return _owner;
}
