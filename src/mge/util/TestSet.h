#ifndef TESTSET_H
#define TESTSET_H

#include <string>

struct TestSet {

public:
	TestSet(); //merely a contain class
	~TestSet();

	std::string FileName;
	unsigned int TotalObjects;
	float StaticObjects;
	float ObbCollider;
	unsigned int OctreeDepth;
	unsigned int NodeTheshold;
	bool DoubleDispatching;
	unsigned int OctreeVersion;
	bool DirtyFlag;
	bool RangeCheck;

	TestSet* copy();
};

#endif