#include <iostream>
#include <string>
#include "OctreeWorld.h"
#include "Octree.h"
#include "mge/octree/BoundingBox.h"

OctreeWorld::OctreeWorld():World() {
	_octreeHalfSize = glm::vec3(50, 50, 50);

	_octree = new Octree(new BoundingBox(glm::vec3(0, 0, 0), _octreeHalfSize), 3);
}

OctreeWorld::~OctreeWorld() {
	delete _octree;
}

void OctreeWorld::update(float step) {
	GameObject::update(step); //base class update

	updateOctree(); //update octree every frame (costly)
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

void OctreeWorld::renderOctree(const glm::mat4 & pModelMatrix, const glm::mat4 & pViewMatrix, const glm::mat4 & pProjectionMatrix) {
	_octree->render(pModelMatrix, pViewMatrix, pProjectionMatrix);
}

glm::vec3 OctreeWorld::getOctreeHalfSize() {
	return _octreeHalfSize;
}
