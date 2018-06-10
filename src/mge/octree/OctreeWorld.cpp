#include "OctreeWorld.h"
#include "Octree.h"

OctreeWorld::OctreeWorld():World() {
	float halfSize = 100.0f;
	float height = 20.0f;

	_octree = new Octree(halfSize, height);
}

OctreeWorld::~OctreeWorld() {
	delete _octree;
}

void OctreeWorld::add(GameObject * pChild) {
	//place reference of object in the octree (and maybe resize it)

	World::add(pChild); //calling the base class function
}
