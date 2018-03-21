#include "mge/behaviours/LightControlBehaviour.hpp"
#include "mge/core/GameObject.hpp"
#include "mge/core/Light.hpp"
#include <SFML/Window/Keyboard.hpp>

LightControlBehaviour::LightControlBehaviour(Light* pLight, float pMoveSpeed) : AbstractBehaviour(), _light(pLight), _moveSpeed(pMoveSpeed), _startTransform(pLight->getTransform()) {
	//constructor
}

LightControlBehaviour::~LightControlBehaviour() {
	//destructor
}

void LightControlBehaviour::update(float pStep) {
	//rotating around (for directional light)
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) _owner->rotate(pStep, glm::vec3(0.0f, 1.0f, 0.0f));
	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) _owner->rotate(-pStep, glm::vec3(0.0f, 1.0f, 0.0f));
	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) _owner->rotate(-pStep, glm::vec3(1.0f, 0.0f, 0.0f));
	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) _owner->rotate(pStep, glm::vec3(1.0f, 0.0f, 0.0f));

	/**
	//simple movement (for point light)
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) _owner->translate(glm::vec3(0.0f, 0.0f, _moveSpeed * pStep));
	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) _owner->translate(glm::vec3(0.0f, 0.0f, -_moveSpeed*pStep));
	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) _owner->translate(glm::vec3(_moveSpeed*pStep, 0.0f, 0.0f));
	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) _owner->translate(glm::vec3(-_moveSpeed * pStep, 0.0f, 0.0f));
	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) _owner->translate(glm::vec3(0.0f, _moveSpeed*pStep, 0.0f));
	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) _owner->translate(glm::vec3(0.0f, -_moveSpeed * pStep, 0.0f));
	/**/

	//reset light position and rotation
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::R)) _owner->setTransform(_startTransform);

	//switch light type
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::J)) _light->setLightType(LightType::DIRECTIONAL);
	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::K)) _light->setLightType(LightType::POINT);
	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::L)) _light->setLightType(LightType::SPOT);
}