#include <iostream>
#include <string>
#include <stdlib.h>

#include "glm.hpp"

#include "mge/core/Renderer.hpp"

#include "mge/core/Mesh.hpp"
#include "mge/core/World.hpp"
#include "mge/core/Texture.hpp"
#include "mge/core/Light.hpp"
#include "mge/core/Camera.hpp"
#include "mge/core/GameObject.hpp"

#include "mge/materials/AbstractMaterial.hpp"
#include "mge/materials/ColorMaterial.hpp"
#include "mge/materials/TextureMaterial.hpp"
#include "mge/materials/LitMaterial.h"
#include "mge/materials/TerrainMaterial.h"

#include "mge/behaviours/RotatingBehaviour.hpp"
#include "mge/behaviours/KeysBehaviour.hpp"
#include "mge/behaviours/LightControlBehaviour.hpp"
#include "mge/behaviours/MovingBehaviour.hpp"

#include "mge/octree/BoundingBox.h"
#include "mge/octree/AABB.h"
#include "mge/octree/OBB.h"

#include "mge/util/DebugHud.hpp"

#include "mge/octree/OctreeWorld.h"
#include "mge/octree/Octree.h"

#include "mge/config.hpp"
#include "mge/MGEDemo.hpp"

#include "mge/util/TestLog.h"

//construct the game class into _window, _renderer and hud (other parts are initialized by build)
MGEDemo::MGEDemo() :AbstractGame(), _hud(0) {
}

MGEDemo::~MGEDemo() {
}

void MGEDemo::initialize() {
	//setup the core part
	AbstractGame::initialize();

	//setup the custom part so we can display some text
	std::cout << "Initializing HUD" << std::endl;
	_hud = new DebugHud(_window);
	std::cout << "HUD initialized." << std::endl << std::endl;
}

//build the game _world
void MGEDemo::_initializeScene() {
	//Camera
	Camera* camera = new Camera("camera", glm::vec3(0, 0, 150));
	camera->setBehaviour(new KeysBehaviour(20.0f)); //movement for debug
	_world->add(camera);
	_world->setMainCamera(camera);

	//Testing
	_initTest(); //init the testobjects
}

void MGEDemo::_render() {
	AbstractGame::_render();
	_updateHud();
}

void MGEDemo::_updateHud() {
	std::string debugInfo = "";

	//draw info from the tester class
	debugInfo += std::string("FPS: ") + std::to_string(TestLog::FPS) + "\n";
	debugInfo += std::string("Objects: ") + std::to_string(TestLog::OBJECTS) + "\n";
	debugInfo += std::string("Octree Depth: ") + std::to_string(TestLog::OCTREE_DEPTH) + "\n";
	debugInfo += std::string("Octree Updates: ") + std::to_string(TestLog::OCTREE_UPDATES) + "\n";
	debugInfo += std::string("Collision Checks: ") + std::to_string(TestLog::COLLISION_CHECKS) + "\n";
	debugInfo += std::string("Collisions: ") + std::to_string(TestLog::COLLISIONS) + "\n";
	debugInfo += std::string("Fit Tests: ") + std::to_string(TestLog::FIT_TESTS) + "\n";
	//debugInfo += std::string("Overlap Tests: ") + std::to_string(TestLog::overlapTests) + "\n";
	debugInfo += std::string("Time: ") + std::to_string(TestLog::time()) + "\n\n";

	TestLog::RESULT_INFO = debugInfo;

	_hud->setDebugInfo(debugInfo);
	_hud->draw();
}

void MGEDemo::_initTest() {
	//Testing
	TestLog::OBJECTS = 100;

	glm::vec3 octreeHalfSize = _world->OCTREE_HALF_SIZE;
	glm::vec3 objectHalfSize = glm::vec3(0.5, 0.5, 0.5);
	float speed = 5.0f;

	srand(0); //seed for the randomizer

	//create testing objects
	for(unsigned i = 0; i < TestLog::OBJECTS; i++) {
		//randomize position based on the octree bounds
		float xPos = rand() % (int)(octreeHalfSize.x * 2) - octreeHalfSize.x;
		float yPos = rand() % (int)(octreeHalfSize.y * 2) - octreeHalfSize.y;
		float zPos = rand() % (int)(octreeHalfSize.z * 2) - octreeHalfSize.z;

		glm::vec3 objectPos = glm::vec3(xPos, yPos, zPos);

		//randomize direction and speed
		float xDir = rand() % 5 - 2.5f;
		float yDir = rand() % 5 - 2.5f;
		float zDir = rand() % 5 - 2.5f;

		//dont allow 0 magnitude directions
		while(xDir == 0 && yDir == 0 && zDir == 0) {
			xDir = rand() % 5 - 2.5f;
			yDir = rand() % 5 - 2.5f;
			zDir = rand() % 5 - 2.5f;
		}

		glm::vec3 movementDirection = glm::vec3(xDir, yDir, zDir);

		//create test object
		GameObject* newCube = new GameObject("Cube " + std::to_string(i), objectPos);
		newCube->setBoundingBox(new AABB(newCube, objectHalfSize)); //add collider to make it work
		newCube->setBehaviour(new MovingBehaviour(movementDirection, speed, octreeHalfSize));

		_world->add(newCube); //also adding to the octree
	}
}

void MGEDemo::_processEvents() {
	sf::Event event;
	bool exit = false;

	//we must empty the event queue
	while(_window->pollEvent(event)) {
		//give all system event listeners a chance to handle events
		//optionally to be implemented by you for example you could implement a
		//SystemEventDispatcher / SystemEventListener pair which allows Listeners to
		//register with the dispatcher and then do something like:
		//SystemEventDispatcher::dispatchEvent(event);

		switch(event.type) {
			case sf::Event::Closed:
				exit = true;
				break;
			case sf::Event::KeyPressed:
				if(event.key.code == sf::Keyboard::Escape) {
					exit = true;
				}

				//Add own key press events here
				break;
			case sf::Event::Resized:
				//would be better to move this to the renderer
				//this version implements nonconstrained match viewport scaling
				glViewport(0, 0, event.size.width, event.size.height);
				break;

			default:
				break;
		}
	}

	if(exit) {
		_window->close();
	}
}