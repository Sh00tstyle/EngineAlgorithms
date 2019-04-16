#ifndef OCTREE_H
#define OCTREE_H

#include <vector>
#include <map>

class BoundingBox;
class GameObject;
class OctreeWorld;
class LineRenderer;
class AbstractBehaviour;

class Octree {
	public:
		Octree(BoundingBox* pBounds, int pDepth = 0, Octree* pParentNode = nullptr); //version 1 constructor
		Octree(int pDepth = 0, Octree* pParentNode = nullptr); //version 2 constructor
		~Octree();

		void addObject(GameObject* newObject);

		//version 1: all nodes exist from the start and get cleared and refilled every frame
		bool fillNodes(GameObject* gameObject, bool checked = false);
		void clearObjects();

		//verison 2: the entire tree is trashed and rebuilt every frame
		void buildTree(BoundingBox* bounds, std::vector<GameObject*> dynamicObjects);
		void trashTree();

		//version 3: keep the tree and update the nodes
		void updateNodes();

		//collision
		void checkCollisions(std::vector<GameObject*> parentObjects); //important to pass by value and not by reference or pointer to get a copy we can safely modify
		void evaluateCollisionStates();

		//rendering
		void render(const glm::mat4& pModelMatrix, const glm::mat4& pViewMatrix, const glm::mat4& pProjectionMatrix);

		//dynamic and static objects
		void filterStatics();

	protected:
		static int _TOTAL_DEPTH; //remembering the total depth of the entire octree
		static int _NODE_TRESHOLD; //treshold at which we dont subdivide the node yet

		//general octree properties
		BoundingBox* _bounds; //define the enclosing region 
		Octree* _childNodes[8]; //eight possible nodes
		Octree* _parentNode; //reference to the parent node, is nullptr if it is the root note
		std::vector<GameObject*> _dynamicObjects; //list of references to the objects that are stored in this node
		std::vector<GameObject*> _staticObjects;
		int _depth; //keeping track of the depth of this node

		//octree updates
		int _maxLifetime;
		int _lifetime;
		bool _hasChildren;

		//octree visualization
		LineRenderer* _octantRenderer;

		//version 1: this builds the nodes when creating a new octree
		void _initOctree(int depth);

		//version 3: inserting the moved objects back into the octree
		void _insert(GameObject* movedObject);
		void _initNode(BoundingBox* bounds, GameObject* item);

		//reset collision states to false and evaluate them at the start and at the end of the frame respectively
		void _resetCollisionStates();

		//cleanup
		void _destructOctree();

};

#endif