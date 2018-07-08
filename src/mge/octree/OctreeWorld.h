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

		void updateOctree();
		void buildOctree();
		void updateNodes();
		void renderOctree(const glm::mat4& pModelMatrix, const glm::mat4& pViewMatrix, const glm::mat4& pProjectionMatrix);

		static glm::vec3 OCTREE_HALF_SIZE;

	private:
		Octree* _octree;
};

#endif