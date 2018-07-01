#ifndef OCTREE_H
#define OCTREE_H

#include <vector>;

class BoundingBox;
class GameObject;

class Octree {
	public:
		Octree(BoundingBox* pBounds, int pDepth = 0, Octree* pParentNode = nullptr);
		~Octree();

		void addObject(GameObject* newObject);
		void updateTree();
		

	private:
		static int TOTAL_DEPTH;

		BoundingBox* _bounds;
		Octree* _childNodes[8]; //eight possible nodes
		Octree* _parentNode;
		std::vector<GameObject*> _objects;
		int _depth;

		void initOctree(int depth);
		void destructTree();

};

#endif