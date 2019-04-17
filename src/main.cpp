#include <iostream>

#include "mge/core/AbstractGame.hpp"
#include "mge/MGEDemo.hpp"

#include "mge/util/TestLog.h"
#include "mge/util/TestSet.h"
#include "mge/config.hpp"

/**
 * Main entry point for the Micro Engine.

 * Design goals:
 * - correct division of OpenGL into appropriate wrapper classes
 * - simple design
 * - each class should know as little as possible about other classes and non related parts of OpenGL
 * - the engine must allow different objects with different transforms, meshes, materials (shaders) etc
 * - consistent coding conventions
 * - reusable and extendable core set of classes which should require no modification
 *   (in other words it is possible to have a working "empty" example)
 *
 * All documentation is contained within the HEADER files, not the CPP files if possible.
 *
 */
int main()
{
    std::cout << "Starting Tests" << std::endl;

	if(!TestLog::readConfigFile(config::OCTREE_CFG_PATH, "config.txt")) {
		
		std::cout << "Cannot test without testsets" << std::endl;

		TestLog::writeConfigFile(config::OCTREE_CFG_PATH, "config.txt");

		int blocker;
		std::cin >> blocker; //block the application to read the console info

		return 0;
	}

	for(unsigned int i = 0; i < TestLog::TESTSETS.size(); ++i) {
		TestLog::setupTest(i);

		//execute application for each testset
		AbstractGame* game = new MGEDemo();
		game->initialize();
		game->run();

		delete game;

		std::system("CLS"); //clear the console
	}

	//cleanup
	for(unsigned int i = 0; i < TestLog::TESTSETS.size(); ++i) {
		delete TestLog::TESTSETS[i];
	}

    return 0;
}