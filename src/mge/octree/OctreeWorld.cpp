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

void OctreeWorld::add(GameObject* pChild) {
	//add object to the octree if it has a box collider
	if(pChild->getBoundingBox() != nullptr) _octree->addObject(pChild);

	World::add(pChild); //calling the base class function
}
