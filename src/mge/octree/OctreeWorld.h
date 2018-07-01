#ifndef OCTREEWORLD_H
#define OCTREEWORLD_H

#include "mge/core/World.hpp"

class Octree;
class GameObject;

class OctreeWorld : public World {
	public:
		OctreeWorld();
		~OctreeWorld();

		virtual void update(float step);

		virtual void add(GameObject* pChild);
		void updateOctree();

	private:
		Octree* _octree;

};

#endif