#include <iostream>
#include <string>
#include "OctreeWorld.h"
#include "Octree.h"
#include "mge/octree/BoundingBox.h"

OctreeWorld::OctreeWorld():World() {
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 halfSize = glm::vec3(100, 100, 100);

	_octree = new Octree(new BoundingBox(position, halfSize), 2);
}

OctreeWorld::~OctreeWorld() {
	delete _octree;
}

void OctreeWorld::update(float step) {
	GameObject::update(step); //base class update

	_octree->checkCollisions();
}

void OctreeWorld::add(GameObject* pChild) {
	World::add(pChild); //calling the base class function

	updateOctree(); //update octree everytime an object is added (costly)
}

void OctreeWorld::updateOctree() {
	_octree->clearObjects(); //empty the whole octree

	//reassign all objects to the octree nodes
	for(int i = 0; i < getChildCount(); i++) {
		GameObject* child = getChildAt(i);

		if(child->getBoundingBox() != nullptr) {
			if(!_octree->updateNodes(getChildAt(i))) _octree->addObject(child); //add to the root, if it coudnt be added somewhere else
		}
	}
}