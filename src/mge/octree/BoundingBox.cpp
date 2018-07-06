#include <iostream>
#include "BoundingBox.h"
#include "mge\core\GameObject.hpp"
#include "mge/core/LineRenderer.hpp"
#include "AABB.h"
#include "OBB.h"

BoundingBox::BoundingBox(glm::vec3 pCenter, glm::vec3 pHalfSize) {
	//stationary bounds for the octree
	_owner = nullptr;
	_center = pCenter;
	_halfSize = pHalfSize;
}

BoundingBox::BoundingBox(GameObject* pOwner, glm::vec3 pHalfSize) {
	//dynamic bounds for object that might move (but can be set to static)
	_owner = pOwner;
	_center = _owner->getLocalPosition();
	_halfSize = pHalfSize;
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
	//AABB vs OBB (TODO)
	glm::vec3 oneCenter = one->getCenter(); // object's pos = collider center
	glm::mat4 oneTransform = one->getOwner()->getTransform(); // scaling for halfsize
	glm::vec3 otherCenter = other->getCenter();
	glm::mat4 otherTransform = other->getOwner()->getTransform();

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
	glm::vec3 oneCenter = one->getCenter(); // object's pos = collider center
	glm::mat4 oneTransform = one->getOwner()->getTransform(); // scaling for halfsize
	glm::vec3 otherCenter = other->getCenter();
	glm::mat4 otherTransform = other->getOwner()->getTransform();

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

bool BoundingBox::collidesWith(BoundingBox * other) {
	//exists for the sake of not making the BoundingBox class abstract (bad)
	return false;
}

bool BoundingBox::collidesWith(AABB * other) {
	//exists for the sake of not making the BoundingBox class abstract (bad)
	return false;
}

bool BoundingBox::collidesWith(OBB * other) {
	//exists for the sake of not making the BoundingBox class abstract (bad)
	return false;
}

glm::vec3 BoundingBox::getHalfSize() {
	return _halfSize;
}

glm::vec3 BoundingBox::getCenter() {
	//only update the center if we have an owner and it is not static
	if(_owner != nullptr && !_owner->isStatic()) _center = _owner->getLocalPosition();

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