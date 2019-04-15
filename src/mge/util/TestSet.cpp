#include "TestSet.h"

TestSet::TestSet() {
	FileName = "";
	TotalObjects = 0;

	StaticObjects = 0.0f;
	ObbCollider = 0.0f;
	OctreeDepth = 0;
	NodeTheshold = 0;
	DoubleDispatching = 0;
}

TestSet::~TestSet() {
}

TestSet* TestSet::copy() {
	TestSet* newTestSet = new TestSet();

	newTestSet->TotalObjects = TotalObjects;
	newTestSet->StaticObjects = StaticObjects;
	newTestSet->ObbCollider = ObbCollider;
	newTestSet->OctreeDepth = OctreeDepth;
	newTestSet->NodeTheshold = NodeTheshold;
	newTestSet->DoubleDispatching = DoubleDispatching;

	return newTestSet;
}
