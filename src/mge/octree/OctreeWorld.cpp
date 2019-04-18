#include <iostream>
#include <string>
#include "OctreeWorld.h"
#include "Octree.h"
#include "mge/octree/BoundingBox.h"
#include "mge/util/TestLog.h"

glm::vec3 OctreeWorld::OCTREE_HALF_SIZE;

OctreeWorld::OctreeWorld():World() {
	OCTREE_HALF_SIZE = glm::vec3(50, 50, 50);

	_octree = nullptr;
}

OctreeWorld::~OctreeWorld() {
	delete _octree;
}

void OctreeWorld::update(float step) {
	GameObject::update(step); //call update from base class and update all gameobjects first

	switch(TestLog::OCTREE_VERSION) {
		case 1:
			updateOctree(); //version 1: clear and refill octree lists every frame
			break;

		case 2:
			buildOctree(); //version 2: trash and rebuild entire octree every frame
			break;

		case 3:
			updateNodes(); //version 3: keep the existing tree and update the nodes
			break;

		default:
			//invalid octree state (no update)
			std::cout << "Octree version invalid, use 1, 2 or 3" << std::endl;
			break;
	}

	_octree->checkCollisions(std::vector<GameObject*>()); //pass an empty vector
	_octree->evaluateCollisionStates(); //checks if colliders are not colliding anymore and updates them
}


void OctreeWorld::updateOctree() {
	//build it for the first time when needed (lazy initialization)
	if(_octree == nullptr) _octree = new Octree(new BoundingBox(glm::vec3(0, 0, 0), OCTREE_HALF_SIZE), TestLog::OCTREE_DEPTH);

	_octree->clearObjects(); //empty the whole octree

	//reassign all objects to the octree nodes
	for(int i = 0; i < getChildCount(); i++) {
		GameObject* child = getChildAt(i);

		if(child->getBoundingBox() != nullptr) {
			if(!_octree->fillNodes(child)) _octree->addObject(child); //nothing in the octree could store the object, so store in the root
		}
	}

	_octree->filterStatics();

	++TestLog::OCTREE_UPDATES;
}

void OctreeWorld::buildOctree() {
	if(_octree == nullptr) {
		//build it for the first time when needed (lazy initialization)
		_octree = new Octree();
		_octree->buildTree(new BoundingBox(glm::vec3(0, 0, 0), OCTREE_HALF_SIZE), getChildrenVector());
	} else {
		//trash and rebuild
		_octree->trashTree(); //essentially frees up memory from the old tree
		_octree->buildTree(new BoundingBox(glm::vec3(0, 0, 0), OCTREE_HALF_SIZE), getChildrenVector());
	}

	_octree->filterStatics();

	++TestLog::OCTREE_UPDATES;
}

void OctreeWorld::updateNodes() {
	if(_octree == nullptr) {
		//build it for the first time when needed (lazy initialization)
		_octree = new Octree();
		_octree->buildTree(new BoundingBox(glm::vec3(0, 0, 0), OCTREE_HALF_SIZE), getChildrenVector());
		_octree->filterStatics();
	} else {
		//update the octree nodes
		_octree->updateNodes();
	}

	++TestLog::OCTREE_UPDATES;
}

void OctreeWorld::renderOctree(const glm::mat4 & pModelMatrix, const glm::mat4 & pViewMatrix, const glm::mat4 & pProjectionMatrix) {
	_octree->render(pModelMatrix, pViewMatrix, pProjectionMatrix);
}