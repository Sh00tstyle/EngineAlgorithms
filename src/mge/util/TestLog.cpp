#include "TestLog.h"
#include <iostream>
#include <fstream>

unsigned int TestLog::FPS = 0;
unsigned int TestLog::OBJECTS = 0;
unsigned int TestLog::OCTREE_DEPTH = 0;
unsigned int TestLog::OCTREE_UPDATES = 0;
unsigned int TestLog::COLLISION_CHECKS = 0;
unsigned int TestLog::COLLISIONS = 0;

unsigned int TestLog::FIT_TESTS = 0;
unsigned int TestLog::OVERLAP_TESTS = 0;

std::string TestLog::RESULT_INFO = "";

std::chrono::steady_clock::time_point TestLog::_START;

void TestLog::start() {
	FPS = 0;
	OBJECTS = 0;
	OCTREE_DEPTH = 0;
	OCTREE_UPDATES = 0;
	COLLISION_CHECKS = 0;
	COLLISIONS = 0;

	FIT_TESTS = 0;
	OVERLAP_TESTS = 0;

	RESULT_INFO = "";

	_START = std::chrono::steady_clock::now();
}

float TestLog::time() {
	std::chrono::duration<double> diff = std::chrono::steady_clock::now() - _START;
	return diff.count();
}

void TestLog::writeToFile() {
	std::cout << "Writing test resutls to testlog.txt" << std::endl;

	std::ofstream file;
	file.open("logs/testlog.txt", std::ios_base::app); //append to file
	file << RESULT_INFO;
	file.close();
}
