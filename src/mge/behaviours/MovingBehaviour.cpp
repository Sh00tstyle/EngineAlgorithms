#include <string>
#include <iostream>
#include "mge/behaviours/MovingBehaviour.hpp"
#include "mge/core/GameObject.hpp"
#include "mge/octree/BoundingBox.h"
#include "mge/core/LineRenderer.hpp"

MovingBehaviour::MovingBehaviour(glm::vec3 pDirection, float pSpeed, glm::vec3 pOctreeBounds, bool pRotate) :AbstractBehaviour() {
	_direction = pDirection;
	_speed = pSpeed;
	_rotate = pRotate;

	_octreeBounds = pOctreeBounds;

	_bounds = nullptr;
}

MovingBehaviour::~MovingBehaviour() {
}

void MovingBehaviour::update(float pStep) {
	if(_owner->isStatic()) return;

	//moves in the given diretion with the given speed
	_owner->translate(_direction * _speed * pStep);

	//rotate 45°/s over the x and y axes
	if(_rotate) _owner->rotate(pStep * glm::radians(45.0f), glm::vec3(1.0f, 1.0f, 0.0f));

	_resolveOutOfBounds();
}

void MovingBehaviour::onCollision(BoundingBox * other) {
	if(_bounds == nullptr) _bounds = _owner->getBoundingBox();

	//cheaply resolve collisions
	glm::vec3 center = _bounds->getCenter();
	glm::vec3 halfSize = _bounds->getHalfSize();
	glm::vec3 otherCenter = other->getCenter();
	glm::vec3 otherHalfSize = other->getHalfSize();

	glm::vec3 translationVec = glm::vec3(0, 0, 0);

	if(center.x >= otherCenter.x) {
		translationVec.x = otherCenter.x + otherHalfSize.x + halfSize.x + 0.5f;
	} else {
		translationVec.x = otherCenter.x - otherHalfSize.x - halfSize.x - 0.5f;
	}

	if(center.y >= otherCenter.y) {
		translationVec.y = otherCenter.y + otherHalfSize.y + halfSize.y + 0.5f;
	} else {
		translationVec.y = otherCenter.y - otherHalfSize.y - halfSize.y - 0.5f;
	}

	if(center.z >= otherCenter.z) {
		translationVec.z = otherCenter.z + otherHalfSize.z + halfSize.z + 0.5f;
	} else {
		translationVec.z = otherCenter.z - otherHalfSize.z - halfSize.z - 0.5f;
	}

	_owner->setLocalPosition(otherCenter + translationVec);
	_direction *= -1;
}

void MovingBehaviour::_resolveOutOfBounds() {
	glm::vec3 pos = _owner->getLocalPosition();
	glm::vec3 boundNormal = glm::vec3(0, 0, 0);

	bool outOfBounds = false;

	//detect out of bounds
	if(pos.x < -_octreeBounds.x) {
		pos.x = -_octreeBounds.x + 0.5f;
		boundNormal.x = 1;
		outOfBounds = true;
	} else if(pos.x > _octreeBounds.x) {
		pos.x = _octreeBounds.x - 0.5f;
		boundNormal.x = -1;
		outOfBounds = true;
	}

	if(pos.y < -_octreeBounds.y) {
		pos.y = -_octreeBounds.y + 0.5f;
		boundNormal.y = 1;
		outOfBounds = true;
	} else if(pos.y > _octreeBounds.y) {
		pos.y = _octreeBounds.y - 0.5f;
		boundNormal.y = -1;
		outOfBounds = true;
	}

	if(pos.z < -_octreeBounds.z) {
		pos.z = -_octreeBounds.z + 0.5f;
		boundNormal.z = 1;
		outOfBounds = true;
	} else if (pos.z > _octreeBounds.z) {
		pos.z = _octreeBounds.z - 0.5f;
		boundNormal.z = -1;
		outOfBounds = true;
	}

	//resolve out of bounds
	if(outOfBounds) {
		_direction = glm::reflect(_direction, glm::normalize(boundNormal));
		_owner->setLocalPosition(pos);
	}
}
