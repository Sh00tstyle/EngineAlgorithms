#ifndef OCTREEWORLD_H
#define OCTREEWORLD_H

#include "mge/core/World.hpp"

class Octree;
class GameObject;

class OctreeWorld : public World {
	public:
		OctreeWorld();
		~OctreeWorld();

		virtual void add(GameObject* pChild);

	private:
		Octree* _octree;

};

#endif