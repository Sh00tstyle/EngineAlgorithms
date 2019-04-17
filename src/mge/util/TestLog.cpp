#include "TestLog.h"
#include <iostream>
#include <fstream>
#include <iterator>
#include <sstream>

#include "TestSet.h"

std::vector<TestSet*> TestLog::TESTSETS;
TestSet* TestLog::CURRENT_TESTSET = nullptr;

unsigned int TestLog::FPS = 0;
unsigned int TestLog::FRAMES = 0;
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
	FRAMES = 0;
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
	return (float)diff.count();
}

void TestLog::setupTest(unsigned int testIndex) {
	start();

	CURRENT_TESTSET = TESTSETS[testIndex];

	std::cout << "Starting test " << testIndex << " for file " << CURRENT_TESTSET->FileName << std::endl;

	TOTAL_OBJECTS = CURRENT_TESTSET->TotalObjects;

	STATIC_OBJECTS = (unsigned int)(TOTAL_OBJECTS * CURRENT_TESTSET->StaticObjects);
	DYNAMIC_OBJECTS = TOTAL_OBJECTS - STATIC_OBJECTS;

	OBB_COLLIDER = (unsigned int)(TOTAL_OBJECTS * CURRENT_TESTSET->ObbCollider);
	AABB_COLLIDER = TOTAL_OBJECTS - OBB_COLLIDER;

	OCTREE_DEPTH = CURRENT_TESTSET->OctreeDepth;
	OCTREE_NODE_TRESHOLD = CURRENT_TESTSET->NodeTheshold;

	USE_DOUBLE_DISPATCHING = CURRENT_TESTSET->DoubleDispatching;
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
			fileWrite << "Frames,";
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

		float time = TestLog::time();

		FPS = (unsigned int)(FRAMES / time); //average fps

		fileWrite << "\n";

		fileWrite << std::to_string(FPS) << ",";
		fileWrite << std::to_string(FRAMES) << ",";
		fileWrite << std::to_string(TOTAL_OBJECTS) << ",";
		fileWrite << std::to_string(STATIC_OBJECTS) << ",";
		fileWrite << std::to_string(DYNAMIC_OBJECTS) << ",";
		fileWrite << std::to_string(OBB_COLLIDER) << ",";
		fileWrite << std::to_string(AABB_COLLIDER) << ",";
		fileWrite << std::to_string(OCTREE_DEPTH) << ",";
		fileWrite << std::to_string(OCTREE_NODE_TRESHOLD) << ",";
		fileWrite << std::to_string(OCTREE_UPDATES) << ",";
		fileWrite << std::to_string(COLLISION_CHECKS) << ",";
		fileWrite << std::to_string(COLLISIONS) << ",";
		fileWrite << std::to_string(FIT_TESTS) << ",";
		fileWrite << std::to_string(time) << ",";

		if(USE_DOUBLE_DISPATCHING) {
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
		file << "Total Objects=" << std::to_string(TOTAL_OBJECTS) << "\n\n";

		file << "Default Testset" << "\n";
		file << "Static Objects=" << std::to_string(STATIC_OBJECTS) << "\n";
		file << "OBB Collider=" << std::to_string(OBB_COLLIDER) << "\n";
		file << "Max Octree Depth=" << std::to_string(OCTREE_DEPTH) << "\n";
		file << "Octree Node Threshold=" << std::to_string(OCTREE_NODE_TRESHOLD) << "\n";
		file << "Use Double Dispatching=" << std::to_string(USE_DOUBLE_DISPATCHING) << "\n\n";

		file << "Testset Configs" << "\n";
		file << "Static Objects=-1\n";
		file << "OBB Collider=-1\n";
		file << "Max Octree Depth=-1\n";
		file << "Octree Node Threshold=-1\n";
		file << "Use Double Dispatching=-1";

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

		if(lines.size() > 16) {
			std::cout << "Unable to read file format" << std::endl;

			return false;
		}

		//general: total objects
		std::vector<std::string> values;
		std::string value;
		std::istringstream valueStream(tokens[1]);

		std::vector<unsigned int> totalObjects;
		std::istringstream iss("");
		iss.clear();

		while(std::getline(valueStream, value, ',')) {
			values.push_back(value);
		}

		valueStream.clear();

		for(unsigned int i = 0; i < values.size(); ++i) {
			iss.str(values[i]);
			unsigned int totalObjectsValue;
			iss >> totalObjectsValue; //convert to value

			totalObjects.push_back(totalObjectsValue); //store in vector

			iss.clear();
		}

		//default testset: static objects
		iss.str(tokens[4]);

		float defaultStaticObjects;
		iss >> defaultStaticObjects;

		iss.clear();

		//default testset: obb collider
		iss.str(tokens[6]);

		float defaultObbCollider;
		iss >> defaultObbCollider;

		iss.clear();

		//default testset: octree depth
		iss.str(tokens[8]);

		unsigned int defaultOctreeDepth;
		iss >> defaultOctreeDepth;

		iss.clear();

		//default testset: node threshold
		iss.str(tokens[10]);

		unsigned int defaultNodeThreshold;
		iss >> defaultNodeThreshold;

		iss.clear();

		//default testset: double dispatching
		iss.str(tokens[12]);

		unsigned int defaultDoubleDispatching;
		iss >> defaultDoubleDispatching;

		iss.clear();

		//testset configs: static objects
		values.clear();
		valueStream.str(tokens[15]);

		std::vector<float> staticObjects;

		while(std::getline(valueStream, value, ',')) {
			values.push_back(value);
		}

		valueStream.clear();

		for(unsigned int i = 0; i < values.size(); ++i) {
			iss.str(values[i]);
			float staticObjectsValue;
			iss >> staticObjectsValue; //convert to value

			staticObjects.push_back(staticObjectsValue); //store in vector

			iss.clear();
		}

		//testset configs: obb collider
		values.clear();
		valueStream.str(tokens[17]);

		std::vector<float> obbCollider;

		while(std::getline(valueStream, value, ',')) {
			values.push_back(value);
		}

		valueStream.clear();

		for(unsigned int i = 0; i < values.size(); ++i) {
			iss.str(values[i]);
			float obbColliderValue;
			iss >> obbColliderValue; //convert to value

			obbCollider.push_back(obbColliderValue); //store in vector

			iss.clear();
		}

		//testset configs: octree depth
		values.clear();
		valueStream.str(tokens[19]);

		std::vector<int> octreeDepths;

		while(std::getline(valueStream, value, ',')) {
			values.push_back(value);
		}

		valueStream.clear();

		for(unsigned int i = 0; i < values.size(); ++i) {
			iss.str(values[i]);
			int octreeDepthValue;
			iss >> octreeDepthValue; //convert to value

			octreeDepths.push_back(octreeDepthValue); //store in vector

			iss.clear();
		}

		//testset configs: node theshold
		values.clear();
		valueStream.str(tokens[21]);

		std::vector<int> nodeThesholds;

		while(std::getline(valueStream, value, ',')) {
			values.push_back(value);
		}

		valueStream.clear();

		for(unsigned int i = 0; i < values.size(); ++i) {
			iss.str(values[i]);
			int nodeThresholdValue;
			iss >> nodeThresholdValue; //convert to value

			nodeThesholds.push_back(nodeThresholdValue); //store in vector

			iss.clear();
		}

		//testset configs: double dispatching
		values.clear();
		valueStream.str(tokens[23]);

		std::vector<int> doubleDispatchingValues;

		while(std::getline(valueStream, value, ',')) {
			values.push_back(value);
		}

		valueStream.clear();

		for(unsigned int i = 0; i < values.size(); ++i) {
			iss.str(values[i]);
			int doubleDispatchingValue;
			iss >> doubleDispatchingValue; //convert to value

			doubleDispatchingValues.push_back(doubleDispatchingValue); //store in vector

			iss.clear();
		}

		//create testsets
		TestSet* defaultSet = new TestSet(); //basic testset template
		defaultSet->StaticObjects = defaultStaticObjects;
		defaultSet->ObbCollider = defaultObbCollider;
		defaultSet->OctreeDepth = defaultOctreeDepth;
		defaultSet->NodeTheshold = defaultNodeThreshold;
		defaultSet->DoubleDispatching = defaultDoubleDispatching;

		TESTSETS.clear();
		TestSet* currentTestSet = nullptr;

		for(unsigned int i = 0; i < totalObjects.size(); ++i) {
			//default testsets
			currentTestSet = defaultSet->copy();

			currentTestSet->TotalObjects = totalObjects[i];
			currentTestSet->FileName = "log_default.csv";

			TESTSETS.push_back(currentTestSet);

			//octree depths
			for(unsigned int j = 0; j < octreeDepths.size(); ++j) {
				if(octreeDepths[j] < 0) break; //don't run tests for values lower than 0

				currentTestSet = defaultSet->copy();

				currentTestSet->TotalObjects = totalObjects[i];
				currentTestSet->OctreeDepth = octreeDepths[j];
				currentTestSet->FileName = "log_octreedepth.csv";

				TESTSETS.push_back(currentTestSet);
			}

			//node thresholds
			for(unsigned int j = 0; j < nodeThesholds.size(); ++j) {
				if(nodeThesholds[j] < 0) break; //don't run tests for values lower than 0

				currentTestSet = defaultSet->copy();

				currentTestSet->TotalObjects = totalObjects[i];
				currentTestSet->NodeTheshold = nodeThesholds[j];
				currentTestSet->FileName = "log_nodethreshold.csv";

				TESTSETS.push_back(currentTestSet);
			}

			//static objects
			for(unsigned int j = 0; j < staticObjects.size(); ++j) {
				if(staticObjects[j] < 0) break; //don't run tests for values lower than 0

				currentTestSet = defaultSet->copy();

				currentTestSet->TotalObjects = totalObjects[i];
				currentTestSet->StaticObjects = staticObjects[j];
				currentTestSet->FileName = "log_staticobjects.csv";

				TESTSETS.push_back(currentTestSet);
			}

			//obb collider
			for(unsigned int j = 0; j < obbCollider.size(); ++j) {
				if(obbCollider[j] < 0) break; //don't run tests for values lower than 0

				currentTestSet = defaultSet->copy();

				currentTestSet->TotalObjects = totalObjects[i];
				currentTestSet->ObbCollider = obbCollider[j];
				currentTestSet->FileName = "log_obbcollider.csv";

				TESTSETS.push_back(currentTestSet);
			}

			//double dispatching
			for(unsigned int j = 0; j < doubleDispatchingValues.size(); ++j) {
				if(doubleDispatchingValues[j] < 0) break; //don't run tests for values lower than 0

				currentTestSet = defaultSet->copy();

				currentTestSet->TotalObjects = totalObjects[i];
				currentTestSet->DoubleDispatching = doubleDispatchingValues[j];
				currentTestSet->FileName = "log_doubledispatching.csv";

				TESTSETS.push_back(currentTestSet);
			}
		}

		std::cout << "Read and generated " << TESTSETS.size() << " testsets\n" << std::endl;

		delete defaultSet; //not needed anymore

		return true;
	}

	std::cout << "Unable to open file '" + filename + "' at " + filepath << std::endl;

	return false;
}
