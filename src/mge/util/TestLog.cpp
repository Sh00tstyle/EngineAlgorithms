#include "TestLog.h"
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <sstream>

unsigned int TestLog::FPS = 0;
unsigned int TestLog::TOTAL_OBJECTS = 0;
unsigned int TestLog::STATIC_OBJECTS = 0;
unsigned int TestLog::DYNAMIC_OBJECTS = 0;
unsigned int TestLog::OCTREE_DEPTH = 0;
unsigned int TestLog::OCTREE_NODE_TRESHOLD = 0;
unsigned int TestLog::OCTREE_UPDATES = 0;
unsigned int TestLog::COLLISION_CHECKS = 0;
unsigned int TestLog::COLLISIONS = 0;
unsigned int TestLog::FIT_TESTS = 0;
bool TestLog::USE_DOUBLE_DISPATCHING = false;

std::string TestLog::RESULT_INFO = "";

std::chrono::steady_clock::time_point TestLog::_START;

void TestLog::start() {
	FPS = 0;
	TOTAL_OBJECTS = 0;
	STATIC_OBJECTS = 0;
	DYNAMIC_OBJECTS = 0;
	OCTREE_DEPTH = 0;
	OCTREE_NODE_TRESHOLD = 0;
	OCTREE_UPDATES = 0;
	COLLISION_CHECKS = 0;
	COLLISIONS = 0;
	FIT_TESTS = 0;
	USE_DOUBLE_DISPATCHING = false;

	RESULT_INFO = "";

	_START = std::chrono::steady_clock::now();
}

float TestLog::time() {
	std::chrono::duration<double> diff = std::chrono::steady_clock::now() - _START;
	return diff.count();
}

void TestLog::writeResultsToFile() {
	std::cout << "Writing test resutls to testlog.txt" << std::endl;

	std::ofstream file;
	file.open("logs/testlog.txt", std::ios_base::app); //append to file
	file << RESULT_INFO;
	file.close();
}

void TestLog::writeConfigFile(std::string filepath, std::string filename) {
	//write the settings to the file, if the file does not exist

	std::ofstream file;
	file.open(filepath + filename, std::ios::out);

	if(file.is_open()) {
		std::cout << "Writing config file '" + filename + "' to " + filepath << std::endl;

		//write config values to file
		file << "Total Objects=" << std::to_string(TestLog::TOTAL_OBJECTS) << "\n";
		file << "Static Objects=" << std::to_string(TestLog::STATIC_OBJECTS) << "\n";
		file << "Max Octree Depth=" << std::to_string(TestLog::OCTREE_DEPTH) << "\n";
		file << "Octree Node Threshold=" << std::to_string(TestLog::OCTREE_NODE_TRESHOLD) << "\n";
		file << "Use Double Dispatching=" << std::to_string(TestLog::USE_DOUBLE_DISPATCHING);

		file.close();
	} else {
		std::cout << "Unable to open file '" + filename + "' at " + filepath << std::endl;
	}
}

bool TestLog::readConfigFile(std::string filepath, std::string filename) {
	//returns true, if the file could be read and the values could be stores, otherwide returns false
	std::ifstream file;
	file.open(filepath + filename, std::ios::in);

	if(file.is_open()) {
		std::cout << "Reading config file '" + filename + "' from " + filepath << std::endl;

		//read content by line
		std::vector<std::string> lines;
		std::string line;

		std::cout << std::endl << "Content:" << std::endl;

		while(std::getline(file, line)) {
			lines.push_back(line);

			std::cout << line << std::endl;
		}

		file.close();

		//split by '='
		std::vector<std::string> tokens;
		std::string token;

		for(unsigned int i = 0; i < lines.size(); ++i) {
			std::istringstream tokenStream(lines[i]);

			while(std::getline(tokenStream, token, '=')) {
				tokens.push_back(token);
			}
		}

		if(lines.size() > 10) {
			std::cout << "Unable to read file format" << std::endl;

			return false;
		}

		//convert content to values and store in variables
		std::istringstream iss(tokens[1]);
		int totalObjects;
		iss >> totalObjects;

		iss.clear();

		iss.str(tokens[3]);
		int staticObjects;
		iss >> staticObjects;

		iss.clear();
		
		iss.str(tokens[5]);
		int octreeDepth;
		iss >> octreeDepth;

		iss.clear();

		iss.str(tokens[7]);
		int nodeThreshold;
		iss >> nodeThreshold;

		iss.clear();

		iss.str(tokens[9]);
		int useDoubleDispatching;
		iss >> useDoubleDispatching;

		iss.clear();

		if(staticObjects > totalObjects) staticObjects = totalObjects;

		TOTAL_OBJECTS = totalObjects;
		STATIC_OBJECTS = staticObjects;
		OCTREE_DEPTH = octreeDepth;
		OCTREE_NODE_TRESHOLD = nodeThreshold;
		USE_DOUBLE_DISPATCHING = useDoubleDispatching;

		return true;
	}

	std::cout << "Unable to open file '" + filename + "' at " + filepath << std::endl;

	return false;
}
