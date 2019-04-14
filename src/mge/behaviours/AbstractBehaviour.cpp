#include "AbstractBehaviour.hpp"

#include <algorithm>

AbstractBehaviour::AbstractBehaviour() :_owner(nullptr), isColliding(false), _wasColliding(false) {}

AbstractBehaviour::~AbstractBehaviour() {
	_owner = nullptr;
}

void AbstractBehaviour::setOwner(GameObject* pOwner) {
	_owner = pOwner;
}

void AbstractBehaviour::setCollisionEnter() {
	isColliding = true;

	if(!_wasColliding) {
		onCollisionEnter();

		_wasColliding = true;
	}
}

void AbstractBehaviour::checkCollisionExit() {
	if(_wasColliding && !isColliding) {
		onCollisionExit();

		_wasColliding = false;
	}
}

void AbstractBehaviour::onCollisionEnter() {
	//behaviour has to be defined in subclasses
}

void AbstractBehaviour::onCollisionExit() {
	//behaviour has to be defined in subclasses
}
