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
		void renderOctree(const glm::mat4& pModelMatrix, const glm::mat4& pViewMatrix, const glm::mat4& pProjectionMatrix);

		glm::vec3 getOctreeHalfSize();

	private:
		Octree* _octree;

		glm::vec3 _octreeHalfSize;

};

#endif