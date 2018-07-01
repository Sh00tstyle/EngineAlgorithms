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
	if(_owner != nullptr && !_owner->isStatic()) _center = _owner->getWorldPosition();

	return _center;
}

glm::vec3 BoundingBox::getMin() {
	return getCenter() - getHalfSize();
}

glm::vec3 BoundingBox::getMax() {
	return getCenter() + getHalfSize();
}

bool BoundingBox::fullyContains(BoundingBox * other) {
	glm::vec3 min = getMin();
	glm::vec3 max = getMax();

	glm::vec3 otherMin = other->getMin();
	glm::vec3 otherMax = other->getMax();

	//check if the other bounds are in these bounds (assuming AABB)
	bool minFits = min.x <= otherMin.x && min.y <= otherMin.y && min.z <= otherMin.z;
	bool maxFits = max.y >= otherMax.x && max.y >= otherMax.y && max.z >= otherMax.z;

	return minFits && maxFits; //returns true, if the other box fully fits into this one
}

bool BoundingBox::partlyContains(BoundingBox * other) {
		glm::vec3 min = getMin();
		glm::vec3 max = getMax();

		glm::vec3 otherCenter = other->getCenter();

		//check if the other center is in these bounds (assuming AABB)
		bool minFits = min.x <= otherCenter.x && min.y <= otherCenter.y && min.z <= otherCenter.z;
		bool maxFits = max.y >= otherCenter.x && max.y >= otherCenter.y && max.z >= otherCenter.z;

		return minFits && maxFits; //returns true, if the other box partially fits into this one
}

GameObject* BoundingBox::getOwner() {
	return _owner;
}
