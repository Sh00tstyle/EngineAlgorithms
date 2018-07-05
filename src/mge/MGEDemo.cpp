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

#include "mge/util/DebugHud.hpp"

#include "mge/octree/OctreeWorld.h"
#include "mge/octree/Octree.h"

#include "mge/config.hpp"
#include "mge/MGEDemo.hpp"

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
	//Meshes
	Mesh* cubeMeshF = Mesh::load(config::MGE_MODEL_PATH + "cube_flat.obj");

	//Materials
	AbstractMaterial* whiteColorMat = new ColorMaterial(glm::vec3(1, 1, 1));

	//Gameobjects
	Camera* camera = new Camera("camera", glm::vec3(0, 0, 150));
	camera->setBehaviour(new KeysBehaviour(10.0f));
	_world->add(camera);
	_world->setMainCamera(camera);

	//Testing
	int cubeAmt = 80;
	glm::vec3 octreeHalfSize = _world->getOctreeHalfSize();
	glm::vec3 pos;
	glm::vec3 dir;
	float speed = 10.0f;

	srand(0); //seed for the randomizer

	//create testing objects
	for(int i = 0; i < cubeAmt; i++) {
		//randomize position based on the octree bounds
		int xPos = rand() % (int)(octreeHalfSize.x * 2) - octreeHalfSize.x;
		int yPos = rand() % (int)(octreeHalfSize.y * 2) - octreeHalfSize.y;
		int zPos = rand() % (int)(octreeHalfSize.z * 2) - octreeHalfSize.z;

		pos = glm::vec3(xPos, yPos, zPos);

		//randomize direction and speed
		int xDir = rand() % 5 - 2;
		int yDir = rand() % 5 - 2;
		int zDir = rand() % 5 - 2;

		//dont allow 0 magnitude directions
		while(xDir == 0 && yDir == 0 && zDir == 0) {
			xDir = rand() % 2 - 1;
			yDir = rand() % 2 - 1;
			zDir = rand() % 2 - 1;
		}

		dir = glm::vec3(xDir, yDir, zDir);

		GameObject* newCube = new GameObject("Cube " + std::to_string(i), pos);
		newCube->setBoundingBox(new BoundingBox(newCube, glm::vec3(1, 1, 1))); //add collider to make it work
		newCube->setMovingBehaviour(new MovingBehaviour(dir, speed, octreeHalfSize)); //just a reference holder basically
		newCube->setBehaviour(newCube->getMovingBehaviour()); //gets updated

		_world->add(newCube); //also adding to the octree
	}
}

void MGEDemo::_render() {
	AbstractGame::_render();
	_updateHud();
}

void MGEDemo::_updateHud() {
	std::string debugInfo = "";
	debugInfo += std::string("FPS:") + std::to_string((int)_fps) + "\n";

	_hud->setDebugInfo(debugInfo);
	_hud->draw();
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