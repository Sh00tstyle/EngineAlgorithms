#ifndef TESTLOG_H
#define TESTLOG_H

#include <chrono>
#include <string>
#include <vector>

struct TestSet;

class TestLog {

	public:
		static std::vector<TestSet*> TESTSETS;
		static TestSet* CURRENT_TESTSET;

		static unsigned int FPS; //frames per second
		static unsigned int FRAMES; //total frame count
		static unsigned int TOTAL_OBJECTS; //total amount of test cubes
		static unsigned int STATIC_OBJECTS; //amount of static cubes
		static unsigned int DYNAMIC_OBJECTS; //amount of dynamic cubes
		static unsigned int OBB_COLLIDER; //amount of OBB collider
		static unsigned int AABB_COLLIDER; //amount of AABB collider
		static unsigned int OCTREE_DEPTH; //intersection depth of the octree
		static unsigned int OCTREE_UPDATES; //amount of updates
		static unsigned int COLLISION_CHECKS; //amount of collision checks
		static unsigned int COLLISIONS; //amount of collisions
		static unsigned int OCTREE_NODE_TRESHOLD; //the maximum amount of objects a node can store before subdividing
		static unsigned int FIT_TESTS; //amount if tests wether an objects fits in an octant of the octree
		static bool USE_DOUBLE_DISPATCHING; //using either double dispatching or a switch case for collision detection
		static unsigned int OCTREE_VERSION; //specifies which iteration of the octree should be used
		static bool USE_DIRTY_FLAG; //determines if dirty flag optimization for collider positions is used
		static bool USE_RANGE_CHECK; //determines if the range check optimization before collision detection is used

		static void start(); //initialize/reset the test class
		static float time(); //returns the time that has elapsed since the start

		static void setupTest(unsigned int testIndex);

		static void writeResultsToFile(std::string filepath, std::string filename);
		static void writeConfigFile(std::string filepath, std::string filename);
		static bool readConfigFile(std::string filepath, std::string filename);

	private:
		static std::chrono::steady_clock::time_point _START;

};

#endif