#include "BoundingBox.h"

#include <iostream>
#include <glm/gtx/norm.hpp> //squared distance

#include "mge\core\GameObject.hpp"
#include "mge/core/LineRenderer.hpp"
#include "AABB.h"
#include "OBB.h"
#include "mge/util/TestLog.h"

const glm::mat4 BoundingBox::_AABB_AXES = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

BoundingBox::BoundingBox(glm::vec3 pCenter, glm::vec3 pHalfSize, unsigned int pType) : _type(pType), _isDirty(true) {
	//stationary bounds for the octree
	_owner = nullptr;
	_halfSize = pHalfSize;
	
	_center = pCenter;

	_cleanDirtyFlag();
}

BoundingBox::BoundingBox(GameObject* pOwner, glm::vec3 pHalfSize, unsigned int pType) : _type(pType), _isDirty(true) {
	//dynamic bounds for object that might move (but can be set to static)
	_owner = pOwner;
	_halfSize = pHalfSize;

	float distanceCheckRadius = _halfSize.x * 2.0f; //"real" minimum distance required to allow collision checks
	_minSqrDistance = distanceCheckRadius * distanceCheckRadius; //squared minimum distance to allow collision checks

	_cleanDirtyFlag();
}

BoundingBox::~BoundingBox() {
}

bool BoundingBox::isColliding(AABB * one, AABB * other) {
	//AABB vs AABB 
	glm::vec3 oneMin = one->getMin();
	glm::vec3 oneMax = one->getMax();

	glm::vec3 otherMin = other->getMin();
	glm::vec3 otherMax = other->getMax();

	return (oneMax.x > otherMin.x &&
			oneMin.x < otherMax.x &&
			oneMax.y > otherMin.y &&
			oneMin.y < otherMax.y &&
			oneMax.z > otherMin.z &&
			oneMin.z < otherMax.z);
}

bool BoundingBox::isColliding(AABB * one, OBB * other) {
	//AABB vs OBB
	glm::vec3 oneCenter = one->getCenter();
	glm::mat3 oneTransform = glm::scale(_AABB_AXES, one->getHalfSize()); //no need to use the transform, since its axis alligned anyway
	glm::vec3 otherCenter = other->getCenter();
	glm::mat4 otherTransform = glm::scale(other->getOwner()->getTransform(), other->getHalfSize());

	for(int a = 0; a < 3; a++) {
		glm::vec3 l = glm::vec3(oneTransform[a]); // one axis to project on
		float tl = std::abs(glm::dot(l, otherCenter) - glm::dot(l, oneCenter)); // center distance
		float ra = std::abs(glm::dot(l, glm::vec3(oneTransform[0]))) + std::abs(glm::dot(l, glm::vec3(oneTransform[1]))) + std::abs(glm::dot(l, glm::vec3(oneTransform[2])));
		float rb = std::abs(glm::dot(l, glm::vec3(otherTransform[0]))) + std::abs(glm::dot(l, glm::vec3(otherTransform[1]))) + std::abs(glm::dot(l, glm::vec3(otherTransform[2])));
		float penetration = (ra + rb) - tl;
		if(penetration <= 0) { // no overlap
			return false;
		}
	}

	for(int b = 0; b < 3; b++) {
		glm::vec3 l = glm::vec3(otherTransform[b]); // other axis to project on
		float tl = std::abs(glm::dot(l, otherCenter) - glm::dot(l, oneCenter)); // center distance
		float ra = std::abs(glm::dot(l, glm::vec3(oneTransform[0]))) + std::abs(glm::dot(l, glm::vec3(oneTransform[1]))) + std::abs(glm::dot(l, glm::vec3(oneTransform[2])));
		float rb = std::abs(glm::dot(l, glm::vec3(otherTransform[0]))) + std::abs(glm::dot(l, glm::vec3(otherTransform[1]))) + std::abs(glm::dot(l, glm::vec3(otherTransform[2])));
		float penetration = (ra + rb) - tl;
		if(penetration <= 0) { // no overlap
			return false;
		}
	}

	for(int a = 0; a < 3; a++) {
		glm::vec3 aAxis = glm::vec3(oneTransform[a]);
		for(int b = 0; b < 3; b++) {
			glm::vec3 bAxis = glm::vec3(otherTransform[b]);
			if(aAxis != bAxis) {
				glm::vec3 l = glm::cross(aAxis, bAxis); // has flaw when axis are same, result in (0,0,0), solved by if
				float tl = std::abs(glm::dot(l, otherCenter) - glm::dot(l, oneCenter)); // center distance
				float ra = std::abs(glm::dot(l, glm::vec3(oneTransform[0]))) + std::abs(glm::dot(l, glm::vec3(oneTransform[1]))) + std::abs(glm::dot(l, glm::vec3(oneTransform[2])));
				float rb = std::abs(glm::dot(l, glm::vec3(otherTransform[0]))) + std::abs(glm::dot(l, glm::vec3(otherTransform[1]))) + std::abs(glm::dot(l, glm::vec3(otherTransform[2])));
				float penetration = (ra + rb) - tl;
				if(penetration <= 0) { // no overlap
					return false;
				}
			}
		}
	}

	return true;
}

bool BoundingBox::isColliding(OBB * one, OBB * other) {
	//OBB vs OBB
	glm::vec3 oneCenter = one->getCenter();
	glm::mat4 oneTransform = glm::scale(one->getOwner()->getTransform(), one->getHalfSize());
	glm::vec3 otherCenter = other->getCenter();
	glm::mat4 otherTransform = glm::scale(other->getOwner()->getTransform(), other->getHalfSize());

	for(int a = 0; a < 3; a++) {
		glm::vec3 l = glm::vec3(oneTransform[a]); // one axis to project on
		float tl = std::abs(glm::dot(l, otherCenter) - glm::dot(l, oneCenter)); // center distance
		float ra = std::abs(glm::dot(l, glm::vec3(oneTransform[0]))) + std::abs(glm::dot(l, glm::vec3(oneTransform[1]))) + std::abs(glm::dot(l, glm::vec3(oneTransform[2])));
		float rb = std::abs(glm::dot(l, glm::vec3(otherTransform[0]))) + std::abs(glm::dot(l, glm::vec3(otherTransform[1]))) + std::abs(glm::dot(l, glm::vec3(otherTransform[2])));
		float penetration = (ra + rb) - tl;
		if(penetration <= 0) { // no overlap
			return false;
		}
	}

	for(int b = 0; b < 3; b++) {
		glm::vec3 l = glm::vec3(otherTransform[b]); // other axis to project on
		float tl = std::abs(glm::dot(l, otherCenter) - glm::dot(l, oneCenter)); // center distance
		float ra = std::abs(glm::dot(l, glm::vec3(oneTransform[0]))) + std::abs(glm::dot(l, glm::vec3(oneTransform[1]))) + std::abs(glm::dot(l, glm::vec3(oneTransform[2])));
		float rb = std::abs(glm::dot(l, glm::vec3(otherTransform[0]))) + std::abs(glm::dot(l, glm::vec3(otherTransform[1]))) + std::abs(glm::dot(l, glm::vec3(otherTransform[2])));
		float penetration = (ra + rb) - tl;
		if(penetration <= 0) { // no overlap
			return false;
		}
	}

	for(int a = 0; a < 3; a++) {
		glm::vec3 aAxis = glm::vec3(oneTransform[a]);
		for(int b = 0; b < 3; b++) {
			glm::vec3 bAxis = glm::vec3(otherTransform[b]);
			if(aAxis != bAxis) {
				glm::vec3 l = glm::cross(aAxis, bAxis); // has flaw when axis are same, result in (0,0,0), solved by if
				float tl = std::abs(glm::dot(l, otherCenter) - glm::dot(l, oneCenter)); // center distance
				float ra = std::abs(glm::dot(l, glm::vec3(oneTransform[0]))) + std::abs(glm::dot(l, glm::vec3(oneTransform[1]))) + std::abs(glm::dot(l, glm::vec3(oneTransform[2])));
				float rb = std::abs(glm::dot(l, glm::vec3(otherTransform[0]))) + std::abs(glm::dot(l, glm::vec3(otherTransform[1]))) + std::abs(glm::dot(l, glm::vec3(otherTransform[2])));
				float penetration = (ra + rb) - tl;
				if(penetration <= 0) { // no overlap
					return false;
				}
			}
		}
	}

	return true;
}

bool BoundingBox::contains(BoundingBox * bounds, BoundingBox * boxToCheck) {
	//returns true if the bounds fully contains the other bounds
	glm::vec3 min = bounds->getMin();
	glm::vec3 max = bounds->getMax();

	glm::vec3 otherMin = boxToCheck->getMin();
	glm::vec3 otherMax = boxToCheck->getMax();

	TestLog::FIT_TESTS++;

	return max.x >= otherMax.x &&
		max.y >= otherMax.y &&
		max.z >= otherMax.z &&
		min.x <= otherMin.x &&
		min.y <= otherMin.y &&
		min.z <= otherMin.z;
}

void BoundingBox::setDirtyFlag(bool value) {
	_isDirty = value;
}

bool BoundingBox::isInRange(BoundingBox * other) {
	if(!TestLog::USE_RANGE_CHECK) return true; //perform no range check

	float sqrDistance = glm::distance2(getCenter(), other->getCenter());

	if(sqrDistance > _minSqrDistance)
		return false; //not in range
	else
		return true; //in range
}

bool BoundingBox::checkCollision(BoundingBox* other) {
	if(TestLog::USE_DOUBLE_DISPATCHING) {
		return collidesWith(other);
	} else {
		return collidesWithSwitch(other);
	}
}

bool BoundingBox::collidesWith(BoundingBox * other) {
	return false;
}

bool BoundingBox::collidesWith(AABB * other) {
	return false;
}

bool BoundingBox::collidesWith(OBB * other) {
	return false;
}

bool BoundingBox::collidesWithSwitch(BoundingBox * other) {
	return false;
}

glm::vec3 BoundingBox::getHalfSize() {
	return _halfSize;
}

void BoundingBox::setHalfSize(glm::vec3 newHalfSize) {
	_halfSize = newHalfSize;

	_isDirty = true;
}

glm::vec3 BoundingBox::getCenter() {
	if(TestLog::USE_DIRTY_FLAG) {
		_cleanDirtyFlag();
	} else {
		if(_owner != nullptr && !_owner->isStatic()) 
			_center = _owner->getLocalPosition();
	}


	return _center;
}

glm::vec3 BoundingBox::getMin() {
	if(TestLog::USE_DIRTY_FLAG) {
		_cleanDirtyFlag();
	} else {
		if(_owner != nullptr && !_owner->isStatic())
			_min = getCenter() - _halfSize;
	}

	return _min;
}

glm::vec3 BoundingBox::getMax() {
	if(TestLog::USE_DIRTY_FLAG) {
		_cleanDirtyFlag();
	} else {
		if(_owner != nullptr && !_owner->isStatic())
			_max = getCenter() - _halfSize;
	}

	return _max;
}

GameObject* BoundingBox::getOwner() {
	return _owner;
}

unsigned int BoundingBox::getType() {
	return _type;
}

void BoundingBox::_cleanDirtyFlag() {
	//clean the dirty flag if needed and update cached center, min and max positions (only marked as dirty if object moved)
	if(!_isDirty) return;

	if(_owner != nullptr) _center = _owner->getLocalPosition(); //only update center for bounds with an owner (non-octree bounds)

	_min = _center - _halfSize;
	_max = _center + _halfSize;

	_isDirty = false;
}
