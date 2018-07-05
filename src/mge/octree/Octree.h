#ifndef OCTREE_H
#define OCTREE_H

#include <vector>

class BoundingBox;
class GameObject;
class OctreeWorld;
class LineRenderer;

class Octree {
	public:
		Octree(BoundingBox* pBounds, int pDepth = 0, Octree* pParentNode = nullptr);
		~Octree();

		void addObject(GameObject* newObject);
		bool updateNodes(GameObject* gameObject);
		void clearObjects();

		void checkCollisions();
		void render(const glm::mat4& pModelMatrix, const glm::mat4& pViewMatrix, const glm::mat4& pProjectionMatrix);

	private:
		static int TOTAL_DEPTH;

		BoundingBox* _bounds;
		LineRenderer* _octantRenderer;

		Octree* _childNodes[8]; //eight possible nodes
		Octree* _parentNode;
		std::vector<GameObject*> _objects;
		int _depth;

		void _initOctree(int depth);
		void _destructOctree();

		bool _contains(glm::vec3 otherPos);
		bool _isColliding(BoundingBox* one, BoundingBox* other);

};

#endif