#include "AbstractBehaviour.hpp"

AbstractBehaviour::AbstractBehaviour():_owner(nullptr) {}

AbstractBehaviour::~AbstractBehaviour()
{
	_owner = nullptr;
}

void AbstractBehaviour::setOwner (GameObject* pOwner) {
    _owner = pOwner;
}

void AbstractBehaviour::onCollision(BoundingBox * other) {
	//for the sake of not forcing to implement this function in the subclasses
}