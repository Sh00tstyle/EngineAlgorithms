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
unsigned int TestLog::OBB_COLLIDER = 0;
unsigned int TestLog::AABB_COLLIDER = 0;
unsigned int TestLog::OCTREE_DEPTH = 0;
unsigned int TestLog::OCTREE_NODE_TRESHOLD = 0;
unsigned int TestLog::OCTREE_UPDATES = 0;
unsigned int TestLog::COLLISION_CHECKS = 0;
unsigned int TestLog::COLLISIONS = 0;
unsigned int TestLog::FIT_TESTS = 0;
bool TestLog::USE_DOUBLE_DISPATCHING = false;

std::chrono::steady_clock::time_point TestLog::_START;

void TestLog::start() {
	FPS = 0;
	TOTAL_OBJECTS = 0;
	STATIC_OBJECTS = 0;
	DYNAMIC_OBJECTS = 0;
	OBB_COLLIDER = 0;
	AABB_COLLIDER = 0;
	OCTREE_DEPTH = 0;
	OCTREE_NODE_TRESHOLD = 0;
	OCTREE_UPDATES = 0;
	COLLISION_CHECKS = 0;
	COLLISIONS = 0;
	FIT_TESTS = 0;
	USE_DOUBLE_DISPATCHING = false;

	_START = std::chrono::steady_clock::now();
}

float TestLog::time() {
	std::chrono::duration<double> diff = std::chrono::steady_clock::now() - _START;
	return diff.count();
}

void TestLog::writeResultsToFile(std::string filepath, std::string filename) {
	//write the results to a logfile in CSV format and append all logs

	std::ifstream fileRead;
	fileRead.open(filepath + filename, std::ios_base::in);

	bool generateHeader = true;

	if(fileRead.is_open()) {
		std::cout << "Reading log file '" + filename + "' from " + filepath << std::endl;

		std::string line;
		int counter = 0;

		while(std::getline(fileRead, line)) {
			if(counter > 1) { //the header exists if there are at least two lines in the log
				generateHeader = false;

				std::cout << "Not generating header file, it already exists" << std::endl;
				break;
			}

			counter++;
		}

		fileRead.close();
	} else {
		std::cout << "Unable to open file '" + filename + "' at " + filepath << std::endl;
	}

	std::ofstream fileWrite;
	fileWrite.open(filepath + filename, std::ios_base::app); //append to file

	if(fileWrite.is_open()) {
		std::cout << "Writing log file '" + filename + "' to " + filepath << std::endl;

		if(generateHeader) {
			fileWrite << "sep=,\n"; //tells excel to seperate by comma
			fileWrite << "FPS,";
			fileWrite << "Total Objects,";
			fileWrite << "Static Objects,";
			fileWrite << "Dynamic Objects,";
			fileWrite << "OBB Collider,";
			fileWrite << "AABB Collider,";
			fileWrite << "Octree Depth,";
			fileWrite << "Octree Node Treshold,";
			fileWrite << "Octree Updates,";
			fileWrite << "Collision Checks,";
			fileWrite << "Collisions,";
			fileWrite << "Fit Tests,";
			fileWrite << "Time,";
			fileWrite << "Double dispatching";
		}

		fileWrite << "\n";

		fileWrite << std::to_string(TestLog::FPS) << ",";
		fileWrite << std::to_string(TestLog::TOTAL_OBJECTS) << ",";
		fileWrite << std::to_string(TestLog::STATIC_OBJECTS) << ",";
		fileWrite << std::to_string(TestLog::DYNAMIC_OBJECTS) << ",";
		fileWrite << std::to_string(TestLog::OBB_COLLIDER) << ",";
		fileWrite << std::to_string(TestLog::AABB_COLLIDER) << ",";
		fileWrite << std::to_string(TestLog::OCTREE_DEPTH) << ",";
		fileWrite << std::to_string(TestLog::OCTREE_NODE_TRESHOLD) << ",";
		fileWrite << std::to_string(TestLog::OCTREE_UPDATES) << ",";
		fileWrite << std::to_string(TestLog::COLLISION_CHECKS) << ",";
		fileWrite << std::to_string(TestLog::COLLISIONS) << ",";
		fileWrite << std::to_string(TestLog::FIT_TESTS) << ",";
		fileWrite << std::to_string(TestLog::time()) << ",";

		if(TestLog::USE_DOUBLE_DISPATCHING) {
			fileWrite << "true";
		} else {
			fileWrite << "false";
		}

		fileWrite.close();
	} else {
		std::cout << "Unable to open file '" + filename + "' at " + filepath << std::endl;
	}
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
		file << "OBB Collider=" << std::to_string(TestLog::OBB_COLLIDER) << "\n";
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

		if(lines.size() > 12) {
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
		int obbCollider;
		iss >> obbCollider;

		iss.clear();
		
		iss.str(tokens[7]);
		int octreeDepth;
		iss >> octreeDepth;

		iss.clear();

		iss.str(tokens[9]);
		int nodeThreshold;
		iss >> nodeThreshold;

		iss.clear();

		iss.str(tokens[11]);
		int useDoubleDispatching;
		iss >> useDoubleDispatching;

		iss.clear();

		if(staticObjects > totalObjects) staticObjects = totalObjects;

		TOTAL_OBJECTS = totalObjects;
		STATIC_OBJECTS = staticObjects;
		DYNAMIC_OBJECTS = TOTAL_OBJECTS - STATIC_OBJECTS;

		if(obbCollider > DYNAMIC_OBJECTS) obbCollider = DYNAMIC_OBJECTS;

		OBB_COLLIDER = obbCollider;
		AABB_COLLIDER = DYNAMIC_OBJECTS - OBB_COLLIDER;
		OCTREE_DEPTH = octreeDepth;
		OCTREE_NODE_TRESHOLD = nodeThreshold;
		USE_DOUBLE_DISPATCHING = useDoubleDispatching;

		return true;
	}

	std::cout << "Unable to open file '" + filename + "' at " + filepath << std::endl;

	return false;
}
