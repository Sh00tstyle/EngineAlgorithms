#ifndef OCTREE_H
#define OCTREE_H

#include <vector>

class BoundingBox;
class GameObject;
class OctreeWorld;
class LineRenderer;

class Octree {
	public:
		Octree(BoundingBox* pBounds, int pDepth = 0, Octree* pParentNode = nullptr); //version 1 constructor
		Octree(int pDepth = 0, Octree* pParentNode = nullptr); //version 2 constructor
		~Octree();

		void addObject(GameObject* newObject);

		//version 1: all nodes exist from the start and get cleared and refilled every frame
		bool updateNodes(GameObject* gameObject, bool checked = false);
		void clearObjects();

		//verison 2: the entire tree is trashed and rebuilt every frame
		void buildTree(BoundingBox* bounds, std::vector<GameObject*> objects);
		void trashTree();

		//collision & rendering
		void checkCollisions();
		void render(const glm::mat4& pModelMatrix, const glm::mat4& pViewMatrix, const glm::mat4& pProjectionMatrix);

	protected:
		static int _TOTAL_DEPTH; //remembering the total depth of the entire octree

		//general octree properties
		BoundingBox* _bounds; //define the enclosing region 
		Octree* _childNodes[8]; //eight possible nodes
		Octree* _parentNode; //reference to the parent node, is nullptr if it is the root note
		std::vector<GameObject*> _objects; //list of references to the objects that are stored in this node
		int _depth; //keeping track of the depth of this node

		//octree visualization
		LineRenderer* _octantRenderer;

		//version 1: this builds the nodes when creating a new octree
		void _initOctree(int depth);

		//collision test
		void _checkCollisions(std::vector<GameObject*> parentObjects); //important to pass by value and not by reference or pointer to get a copy we can safely modify

		//cleanup
		void _destructOctree();

};

#endif