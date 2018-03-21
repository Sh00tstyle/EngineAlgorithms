#include <iostream>
#include <string>

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

#include "mge/util/DebugHud.hpp"

#include "mge/config.hpp"
#include "mge/MGEDemo.hpp"

//construct the game class into _window, _renderer and hud (other parts are initialized by build)
MGEDemo::MGEDemo() :AbstractGame(), _hud(0) {
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
	Mesh* planeMesh = Mesh::load(config::MGE_MODEL_PATH + "plane_8192.obj");
	Mesh* cubeMeshF = Mesh::load(config::MGE_MODEL_PATH + "cube_flat.obj");

	//Materials
	AbstractMaterial* whiteColorMat = new ColorMaterial(glm::vec3(1, 1, 1));
	AbstractMaterial* litMat = new LitMaterial(glm::vec3(0, 1, 1), glm::vec3(1, 0, 1), glm::vec3(1, 1, 1), 256.0f);
	AbstractMaterial* terrainMat = new TerrainMaterial(
		glm::vec3(0.1f, 0.1f, 0.1f), //ambient color
		glm::vec3(1, 1, 1), //specular color
		256.0f, //shininess
		Texture::load(config::MGE_TEXTURE_PATH + "diffuse1.jpg"), //diffuse texture 1
		Texture::load(config::MGE_TEXTURE_PATH + "water.jpg"), //diffuse texture 2
		Texture::load(config::MGE_TEXTURE_PATH + "diffuse3.jpg"), //diffuse texture 3
		Texture::load(config::MGE_TEXTURE_PATH + "diffuse4.jpg"), //diffuse texture 4
		Texture::load(config::MGE_TEXTURE_PATH + "heightmap.png"), //heightmap texture
		Texture::load(config::MGE_TEXTURE_PATH + "splatmap.png") //splatmap texture
	);

	//Gameobjects
	Camera* camera = new Camera("camera", glm::vec3(0, 5, 10));
	camera->rotate(glm::radians(-20.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //rotate by -20° around the x axis
	_world->add(camera);
	_world->setMainCamera(camera);

	GameObject* plane = new GameObject("plane", glm::vec3(0, 0, 0));
	plane->scale(glm::vec3(5, 5, 5));
	plane->setMesh(planeMesh);
	plane->setMaterial(terrainMat);
	plane->setBehaviour(new KeysBehaviour(10)); //to rotate the terrain
	_world->add(plane);

	Light* mainLight = new Light(LightType::DIRECTIONAL, //light type
								 glm::vec3(1, 1, 0), //light color
								 1.0f, //intensity
								 0.5f, //ambientContribution 
								 1.0f, //constantAttenutation
								 0.3f, //linearAttenuation
								 0.0f, //quadraticAttenuation
								 45.0f, //outerConeAngle
								 25.0f, //innerConeAngle
								 "mainLight", //name
								 glm::vec3(0, 6, 0) //position
	);
	mainLight->scale(glm::vec3(0.3f, 0.3f, 0.3f));
	mainLight->setMesh(cubeMeshF);
	mainLight->setBehaviour(new LightControlBehaviour(mainLight, 25));
	_world->add(mainLight); //light gets automatically registered in the world

	GameObject* indicator = new GameObject("indicator", glm::vec3(0, 0, 1.0f));
	indicator->scale(glm::vec3(0.5f, 0.5f, 0.5f));
	indicator->setMesh(cubeMeshF);
	indicator->setMaterial(whiteColorMat);
	mainLight->add(indicator); //adding as a child of light

	_renderer->setClearColor(52, 204, 255, 255); //changing clear color (background) to light blue
}

void MGEDemo::_render() {
	AbstractGame::_render();
	_updateHud();
}

void MGEDemo::_updateHud() {
	std::string debugInfo = "";
	debugInfo += std::string("FPS:") + std::to_string((int)_fps) + "\n";

	//Adding the hierarchy and the parent of everything (the world)
	debugInfo += "\n\n" + std::string("Hierarchy:") + "\n" + _world->getName();

	//getting every children in the world object and printing it
	for(int i = 0; i < _world->getChildCount(); i++) {
		debugInfo += "\n\t" + _world->getChildAt(i)->getName();

		for(int j = 0; j < _world->getChildAt(i)->getChildCount(); j++) {
			debugInfo += "\n\t\t" + _world->getChildAt(i)->getChildAt(j)->getName();

			//could potentially get every children of those gameobjects and print their children too, if there were any but 2 iterations is enough for now
			//solution for more and easier iteration would be recursion
		}
	}

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

MGEDemo::~MGEDemo() {
	//dtor
}
