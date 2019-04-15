#ifndef TESTSET_H
#define TESTSET_H

#include <string>

struct TestSet {

public:
	TestSet(); //merely a contain class
	~TestSet();

	std::string FileName;
	unsigned int TotalObjects;
	unsigned int StaticObjects;
	float ObbCollider;
	float OctreeDepth;
	unsigned int NodeTheshold;
	bool DoubleDispatching;

	TestSet* copy();
};

#endif