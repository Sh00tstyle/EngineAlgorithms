#ifndef TESTLOG_H
#define TESTLOG_H

#include <chrono>
#include <string>

class TestLog {

	public:
		static unsigned int fps; //frames per second
		static unsigned int objects; //amount of test cubes
		static unsigned int octreeDepth; //intersection depth of the octree
		static unsigned int octreeUpdates; //amount of updates
		static unsigned int collisionChecks; //amount of collision checks
		static unsigned int collisions; //amount of collisions

		static unsigned int fitTests; //amount if tests wether an objects fits in an octant of the octree
		static unsigned int overlapTests; //???

		static std::string resultInfo;

		static void start(); //initialize/reset the test class
		static float time(); //returns the time that has elapsed since the start

		static void writeToFile();

	private:
		static std::chrono::steady_clock::time_point _start;

};

#endif