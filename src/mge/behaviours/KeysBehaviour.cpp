#include "mge/behaviours/KeysBehaviour.hpp"
#include "mge/core/GameObject.hpp"
#include <SFML/Window/Keyboard.hpp>

KeysBehaviour::KeysBehaviour(float pMoveSpeed, float pTurnSpeed) : AbstractBehaviour(), _moveSpeed(pMoveSpeed), _turnSpeed(pTurnSpeed) {
}

KeysBehaviour::~KeysBehaviour() {
}

void KeysBehaviour::update(float pStep) {
	//default if no keys
	float xSpeed = 0.0f;
	float ySpeed = 0.0f;
	float zSpeed = 0.0f;
	float yTurnSpeed = 0.0f;
	float xTurnSpeed = 0.0f;

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		zSpeed = -_moveSpeed;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		zSpeed = _moveSpeed;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		xSpeed = -_moveSpeed;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		xSpeed = _moveSpeed;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		ySpeed = _moveSpeed;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
		ySpeed = -_moveSpeed;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		yTurnSpeed = -_turnSpeed;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		yTurnSpeed = +_turnSpeed;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		xTurnSpeed = _turnSpeed;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		xTurnSpeed = -_turnSpeed;
	}

	//translate the object in its own local space
	_owner->translate(glm::vec3(xSpeed * pStep, ySpeed * pStep, zSpeed * pStep));

	//we can also translate directly, basically we take the z axis from the matrix
	//which is normalized and multiply it by moveSpeed*step, then we add it to the
	//translation component
	//glm::mat4 transform = _owner->getTransform();
	//transform[3] += transform[2] * moveSpeed*pStep;
	//_owner->setTransform(transform);

	//rotate the object in its own local space
	_owner->rotate(glm::radians(yTurnSpeed * pStep), glm::vec3(0.0f, 1.0f, 0.0f));
	_owner->rotate(glm::radians(xTurnSpeed * pStep), glm::vec3(1.0f, 0.0f, 0.0f));

	//NOTE:
	//The reason the above happens in the local space of the object and not in the world space
	//is that we use the _owner->translate / rotate shortcuts which in turn call glm::rotate / glm::translate
	//The methods multiply the current transform with the translation/rotation matrix from left to right
	//meaning everything happens in local space.
}
