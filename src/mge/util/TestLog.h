#ifndef TESTLOG_H
#define TESTLOG_H

#include <chrono>
#include <string>

class TestLog {

	public:
		static unsigned int FPS; //frames per second
		static unsigned int OBJECTS; //amount of test cubes
		static unsigned int OCTREE_DEPTH; //intersection depth of the octree
		static unsigned int OCTREE_UPDATES; //amount of updates
		static unsigned int COLLISION_CHECKS; //amount of collision checks
		static unsigned int COLLISIONS; //amount of collisions
		static unsigned int OCTREE_NODE_TRESHOLD; //the minimum amount of objects a node can store before subdividing
		static unsigned int FIT_TESTS; //amount if tests wether an objects fits in an octant of the octree

		static std::string RESULT_INFO;

		static void start(); //initialize/reset the test class
		static float time(); //returns the time that has elapsed since the start

		static void writeToFile();

	private:
		static std::chrono::steady_clock::time_point _START;

};

#endif