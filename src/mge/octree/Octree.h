#ifndef OCTREE_H
#define OCTREE_H

#include <vector>

class BoundingBox;
class GameObject;
class OctreeWorld;

class Octree {
	public:
		Octree(BoundingBox* pBounds, int pDepth = 0, Octree* pParentNode = nullptr);
		~Octree();

		void addObject(GameObject* newObject);
		bool updateNodes(GameObject* gameObject);
		void clearObjects();

		void checkCollisions();

	private:
		static int TOTAL_DEPTH;

		BoundingBox* _bounds;
		Octree* _childNodes[8]; //eight possible nodes
		Octree* _parentNode;
		std::vector<GameObject*> _objects;
		int _depth;

		void initOctree(int depth);
		void destructOctree();

		bool isColliding(BoundingBox* one, BoundingBox* other);

};

#endif