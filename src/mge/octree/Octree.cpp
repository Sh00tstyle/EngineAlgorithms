#include <iostream>
#include <string>
#include "Octree.h"
#include "BoundingBox.h"

int Octree::TOTAL_DEPTH = 0; //init static depth member

Octree::Octree(BoundingBox * pBounds, int pDepth, Octree * pParentNode): _bounds(pBounds), _parentNode(pParentNode) {
	if(_parentNode == nullptr) TOTAL_DEPTH = pDepth; //store total depth to reconstruct octree later
	_depth = TOTAL_DEPTH - pDepth; //depth of the node itself

	initOctree(pDepth); //build octree for the first time, since it is the root node
}

Octree::~Octree() {
	destructTree(); //remove all child nodes if there are any
}

void Octree::addObject(GameObject * newObject) {
	_objects.push_back(newObject);
}

void Octree::updateTree() {
	initOctree(TOTAL_DEPTH);
}

void Octree::initOctree(int depth) {
	std::cout << "Created octree node" << std::endl;

	if(depth <= 0) return; //stop constructing

	//new octant areas for the child nodes
	BoundingBox* areas[8];

	glm::vec3 octantHalfSize = _bounds->getHalfSize() * 0.5f; //half the halfsize to get 
	glm::vec3 center = _bounds->getCenter();

	//define new bounds for new areas
	areas[0] = new BoundingBox(center + glm::vec3(-octantHalfSize.x, -octantHalfSize.y, -octantHalfSize.z), octantHalfSize); //bottom, back, left
	areas[1] = new BoundingBox(center + glm::vec3(octantHalfSize.x, -octantHalfSize.y, -octantHalfSize.z), octantHalfSize); //bottom, back, right
	areas[2] = new BoundingBox(center + glm::vec3(-octantHalfSize.x, -octantHalfSize.y, octantHalfSize.z), octantHalfSize); //bottom, front, left
	areas[3] = new BoundingBox(center + glm::vec3(octantHalfSize.x, -octantHalfSize.y, octantHalfSize.z), octantHalfSize); //bottom, front, right
	areas[4] = new BoundingBox(center + glm::vec3(-octantHalfSize.x, octantHalfSize.y, -octantHalfSize.z), octantHalfSize); //top, back, left
	areas[5] = new BoundingBox(center + glm::vec3(octantHalfSize.x, octantHalfSize.y, -octantHalfSize.z), octantHalfSize); //top, back, right
	areas[6] = new BoundingBox(center + glm::vec3(-octantHalfSize.x, octantHalfSize.y, octantHalfSize.z), octantHalfSize); //top, front, left
	areas[7] = new BoundingBox(center + glm::vec3(octantHalfSize.x, octantHalfSize.y, octantHalfSize.z), octantHalfSize); //top, front, right

	//recursively create new nodes and tell them the new remaining depth
	for(unsigned int i = 0; i < 8; i++) {
		_childNodes[i] = new Octree(areas[i], depth - 1, this);
	}
}

void Octree::destructTree() {
	if(_depth >= TOTAL_DEPTH) return; //dont destruct if there is no depth left

	for(unsigned int i = 0; i < 8; i++) {
		delete _childNodes[i];
	}
}
