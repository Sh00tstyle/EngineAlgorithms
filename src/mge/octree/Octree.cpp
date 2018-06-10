#include <iostream>
#include <string>
#include "Octree.h"
#include "OctreeNode.h"

//basically the root node of the octree
Octree::Octree(float pHalfSize, float pHeight, int pDepth): _halfSize(pHalfSize), _height(pHeight), _depth(pDepth) {
	//printing out octree size
	std::cout << "Octree - halfSize: " + std::to_string(_halfSize) + ", height: " + std::to_string(_height) + ", depth: " + std::to_string(_depth) << std::endl;

	InitOctree();
}

Octree::~Octree() {
	for(int i = 0; i < 8; i++) {
		if(_childNodes[i] == nullptr) delete _childNodes[i];
	}
}

void Octree::InitOctree() {
	for(int i = 0; i < 8; i++) {
		_childNodes[i] = new OctreeNode(nullptr);
	}
}
