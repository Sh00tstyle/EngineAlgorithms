#ifndef OCTREENODE_H
#define OCTREENODE_H

#include <vector>

class GameObject;

class OctreeNode {
	public:
		OctreeNode(OctreeNode* pParentNode);
		~OctreeNode();

		void SetNodeChildAt(OctreeNode* childNode, int arrayPos);

		void AddGameObject(GameObject* object);
		void RemoveGameObject(GameObject* object);
		GameObject* GetGameObjectAt(int index);
		int GetGameObjectsCount();

	private:
		OctreeNode* _parentNode;
		OctreeNode* _childNodes[8]; //eight possible children

		std::vector<GameObject*> _gameObjects;

};

#endif