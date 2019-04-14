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
	_colliderRenderer = nullptr;
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

void MovingBehaviour::onCollisionEnter() {
	if(_colliderRenderer == nullptr) _colliderRenderer = _owner->getColliderRenderer();

	_colliderRenderer->setLineColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); //red, when colliding

	std::cout << _owner->getName() + " is colliding" << std::endl;
}

void MovingBehaviour::onCollisionExit() {
	_colliderRenderer->setLineColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)); //green, when not colliding

	std::cout << _owner->getName() + " is not colliding anymore" << std::endl;
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
