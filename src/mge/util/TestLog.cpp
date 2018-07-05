#include "TestLog.h"
#include <iostream>
#include <fstream>

unsigned int TestLog::fps = 0;
unsigned int TestLog::objects = 0;
unsigned int TestLog::octreeDepth = 0;
unsigned int TestLog::octreeUpdates = 0;
unsigned int TestLog::collisionChecks = 0;
unsigned int TestLog::collisions = 0;

unsigned int TestLog::fitTests = 0;
unsigned int TestLog::overlapTests = 0;

std::string TestLog::resultInfo = "";

std::chrono::steady_clock::time_point TestLog::_start;

void TestLog::start() {
	fps = 0;
	objects = 0;
	octreeDepth = 0;
	octreeUpdates = 0;
	collisionChecks = 0;
	collisions = 0;

	fitTests = 0;
	overlapTests = 0;

	resultInfo = "";

	_start = std::chrono::steady_clock::now();
}

float TestLog::time() {
	std::chrono::duration<double> diff = std::chrono::steady_clock::now() - _start;
	return diff.count();
}

void TestLog::writeToFile() {
	std::cout << "Writing test resutls to testlog.txt" << std::endl;

	std::ofstream file;
	file.open("logs/testlog.txt", std::ios_base::app); //append to file
	file << resultInfo;
	file.close();
}
