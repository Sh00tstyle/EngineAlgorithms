#include <string>
#include <iostream>
#include "mge/behaviours/MovingBehaviour.hpp"
#include "mge/core/GameObject.hpp"
#include "mge/octree/BoundingBox.h"
#include "mge/core/LineRenderer.hpp"

MovingBehaviour::MovingBehaviour(glm::vec3 pDirection, float pSpeed, glm::vec3 pOctreeBounds) :AbstractBehaviour() {
	_direction = pDirection;
	_speed = pSpeed;

	_octreeBounds = pOctreeBounds;

	_bounds = nullptr;
}

MovingBehaviour::~MovingBehaviour() {
}

void MovingBehaviour::update(float pStep) {
	if(_owner->isStatic()) return;

	//moves in the given diretion with the given speed
	_owner->translate(_direction * _speed * pStep);

	if(_isOutOfBounds()) {
		//move teleport object back to a random position inside the bounds
		int xPos = rand() % (int)(_octreeBounds.x * 2) - _octreeBounds.x;
		int yPos = rand() % (int)(_octreeBounds.y * 2) - _octreeBounds.y;
		int zPos = rand() % (int)(_octreeBounds.z * 2) - _octreeBounds.z;

		_owner->setLocalPosition(glm::vec3(xPos, yPos, zPos));

		//invert direction
		_direction *= -1.0f;
	}
}

void MovingBehaviour::onCollision(BoundingBox * other) {
	_direction *= -1.0f;

	if(_bounds == nullptr) _bounds = _owner->getBoundingBox();

	//resolve collision cheaply (only x)
	_owner->translate(glm::vec3(2.0f, 2.0f, 2.0f));
}

bool MovingBehaviour::_isOutOfBounds() {
	//returns true, if the position of the owner is not in the bounds of the octree bounds

	glm::vec3 pos = _owner->getLocalPosition();

	return !(pos.x > -_octreeBounds.x &&
			 pos.x < _octreeBounds.x &&
			 pos.y > -_octreeBounds.y &&
			 pos.y < _octreeBounds.y &&
			 pos.z > -_octreeBounds.z &&
			 pos.z < _octreeBounds.z);
}
