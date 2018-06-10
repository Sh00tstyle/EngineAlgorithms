#include <iostream>
#include <algorithm> //for std::remove
#include "OctreeNode.h"

OctreeNode::OctreeNode(OctreeNode* pParentNode): _parentNode(pParentNode) {
	if(pParentNode == nullptr) std::cout << "root node detected" << std::endl;
}

OctreeNode::~OctreeNode() {
	for(int i = 0; i < 8; i++) {
		if(_childNodes[i] == nullptr) delete _childNodes[i];
	}
}

void OctreeNode::SetNodeChildAt(OctreeNode * childNode, int arrayPos) {
	_childNodes[arrayPos] = childNode;
}

void OctreeNode::AddGameObject(GameObject * object) {
	_gameObjects.push_back(object);
}

void OctreeNode::RemoveGameObject(GameObject * object) {
	_gameObjects.erase(std::remove(_gameObjects.begin(), _gameObjects.end(), object), _gameObjects.end());
}

GameObject * OctreeNode::GetGameObjectAt(int index) {
	return _gameObjects[index];
}

int OctreeNode::GetGameObjectsCount() {
	return _gameObjects.size();
}
