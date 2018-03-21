#include "mge/behaviours/CameraOrbitBehaviour.h"
#include "mge/core/GameObject.hpp"
#include <SFML/Window/Mouse.hpp>
#include <iostream>

CameraOrbitBehaviour::CameraOrbitBehaviour(float pDistance, float pMinAngle, float pMaxAngle, float pRotationSpeed, GameObject* pTargetObject):AbstractBehaviour(), 
_distance(pDistance), _minAngle(pMinAngle), _maxAngle(pMaxAngle), _rotationSpeed(pRotationSpeed), _targetObject(pTargetObject) {
	//constructor

	_prevMousePos = sf::Mouse::getPosition();
	_xAngle = 0.0f;
}

CameraOrbitBehaviour::~CameraOrbitBehaviour() {
	//destructor
	_targetObject = nullptr;
}

void CameraOrbitBehaviour::update(float step) {
	sf::Vector2i currentMousePos = sf::Mouse::getPosition();

	glm::vec3 targetPos = _targetObject->getLocalPosition();
	glm::mat4 cameraMatrix = glm::translate(glm::vec3(0, 0, _distance)); //applying the _distance offset to the camera as a starting position for the camera

	if(currentMousePos.x < _prevMousePos.x) { //mouse moved left	
		_rotationYMatrix = glm::rotate(_rotationYMatrix, _rotationSpeed * step, glm::vec3(0, 1, 0)); //modifying the rotation matrix for the y axis based on the mouse movement
	} else if(currentMousePos.x > _prevMousePos.x) { //mouse moved right
		_rotationYMatrix = glm::rotate(_rotationYMatrix, -_rotationSpeed * step, glm::vec3(0, 1, 0));
	} 
		
	if(currentMousePos.y < _prevMousePos.y && _xAngle < _maxAngle) { //mouse moved up
		_rotationXMatrix = glm::rotate(_rotationXMatrix, step, glm::vec3(1, 0, 0)); //modifying the rotation matrix for the x axis based on the mouse movement
		_xAngle += step; //keeping track of the angle
	} else if(currentMousePos.y > _prevMousePos.y && _xAngle > _minAngle) { //mouse moved down
		_rotationXMatrix = glm::rotate(_rotationXMatrix, -step, glm::vec3(1, 0, 0));
		_xAngle -= step;
	}

	cameraMatrix = _rotationXMatrix * _rotationYMatrix * cameraMatrix; //applying the rotation matrices to the camera matrix (note: if we would multiply it the other way around, the camera would around itself and not around the origin)
	cameraMatrix = glm::translate(targetPos) * cameraMatrix; //translating the camera to the position of the target object while still maintaining the offset

	/**
	//lookAt matrix (view matrix?)
	glm::vec3 cameraPos = cameraMatrix[3];
	glm::vec3 forward = glm::normalize(targetPos - cameraPos);
	glm::vec3 side = glm::normalize(glm::cross(glm::vec3(0, 1, 0), forward));
	glm::vec3 up = glm::normalize(glm::cross(forward, side));

	glm::mat4 lookAt; 

	lookAt[0][0] = side.x;
	lookAt[0][1] = side.y;
	lookAt[0][2] = side.z;

	lookAt[1][0] = up.x;
	lookAt[1][1] = up.y;
	lookAt[1][2] = up.z;

	lookAt[2][0] = forward.x;
	lookAt[2][1] = forward.y;
	lookAt[2][2] = forward.z;

	lookAt[3][0] = cameraPos.x;
	lookAt[3][1] = cameraPos.y;
	lookAt[3][2] = cameraPos.z;

	//what to do with it?
	/**/

	_owner->setTransform(cameraMatrix); //applying the model matrix on the camera

	_prevMousePos = currentMousePos;
}