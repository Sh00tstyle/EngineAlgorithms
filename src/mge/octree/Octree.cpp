#include <iostream>
#include <string>
#include <algorithm>
#include "OctreeWorld.h"
#include "Octree.h"
#include "BoundingBox.h"
#include "AABB.h"
#include "OBB.h"
#include "mge\core/LineRenderer.hpp"
#include "mge/behaviours/MovingBehaviour.hpp"
#include "mge/util/TestLog.h"

int Octree::_TOTAL_DEPTH = 0; //init static depth member
int Octree::_NODE_TRESHOLD = 10; //init static node treshold member

Octree::Octree(BoundingBox * pBounds, int pDepth, Octree * pParentNode) : _bounds(pBounds), _parentNode(pParentNode), _objects() {
	if(_parentNode == nullptr) {
		_TOTAL_DEPTH = pDepth;
		TestLog::OCTREE_DEPTH = _TOTAL_DEPTH;
		_depth = 0;
	} else {
		_depth = _TOTAL_DEPTH - pDepth; //depth of the node itself
	}

	_octantRenderer = new LineRenderer(_bounds, true);
	_octantRenderer->setLineColor(glm::vec4(0.0f, 0.0f, 0.5f, 0.5f)); //blue

	_initOctree(pDepth); //build octree for the first time, since it is the root node
}

Octree::Octree(int pDepth, Octree* pParentNode) : _bounds(nullptr), _parentNode(pParentNode), _octantRenderer(nullptr), _objects() {
	//init everything with null except for the depth, the buildTree function will take care of filling the tree node with relevant data
	if(_parentNode == nullptr) {
		_TOTAL_DEPTH = TestLog::OCTREE_DEPTH;
		_NODE_TRESHOLD = TestLog::OCTREE_NODE_TRESHOLD;
		_depth = 0;
	} else {
		_depth = pDepth; //depth of the node itself
	}

	//default init child nodes
	for(int i = 0; i < 8; i++) {
		_childNodes[i] = nullptr;
	}

	//default starting lifetime
	_maxLifetime = 8;
	_lifetime = -1;
	_hasChildren = false;
}

Octree::~Octree() {
	_destructOctree(); //remove all child nodes if there are any and cleanup pointers
}

void Octree::addObject(GameObject * newObject) {
	_objects.push_back(newObject);
}

//returns true if the node could store the object, otherwise returns false
bool Octree::fillNodes(GameObject* gameObject, bool checked) {
	if(BoundingBox::contains(_bounds, gameObject->getBoundingBox())) {
		if(_depth < _TOTAL_DEPTH) {
			for(int i = 0; i < 8; i++) {
				if(_childNodes[i]->fillNodes(gameObject)) return true;
			}

			//no child contains the object, so store it in this node
			addObject(gameObject);
			return true;
		} else {
			//no more children left and this node contains the object fully, so add it
			addObject(gameObject);
			return true;
		}
	}

	return false;
}

void Octree::buildTree(BoundingBox* bounds, std::vector<GameObject*> objects) {
	//init this node
	_objects = objects;
	_bounds = bounds;
	_octantRenderer = new LineRenderer(bounds, true);
	_octantRenderer->setLineColor(glm::vec4(0.0f, 0.0f, 0.5f, 0.5f)); //blue

	if(_objects.size() <= _NODE_TRESHOLD || _depth >= _TOTAL_DEPTH) return; //terminate recursion, if we reached a leaf node

	//new octant areas for the child nodes
	BoundingBox* areas[8];

	glm::vec3 octantHalfSize = _bounds->getHalfSize() * 0.5f; //half the halfsize to get halfsize for the new octant
	glm::vec3 center = _bounds->getCenter();

	//define new bounds for new areas
	areas[0] = new BoundingBox(center + glm::vec3(-octantHalfSize.x, -octantHalfSize.y, -octantHalfSize.z), octantHalfSize); //bottom, back, left
	areas[1] = new BoundingBox(center + glm::vec3(octantHalfSize.x, -octantHalfSize.y, -octantHalfSize.z), octantHalfSize); //bottom, back, right
	areas[2] = new BoundingBox(center + glm::vec3(-octantHalfSize.x, -octantHalfSize.y, octantHalfSize.z), octantHalfSize); //bottom, front, left
	areas[3] = new BoundingBox(center + glm::vec3(octantHalfSize.x, -octantHalfSize.y, octantHalfSize.z), octantHalfSize); //bottom, front, right
	areas[4] = new BoundingBox(center + glm::vec3(-octantHalfSize.x, octantHalfSize.y, -octantHalfSize.z), octantHalfSize); //top, back, left
	areas[5] = new BoundingBox(center + glm::vec3(octantHalfSize.x, octantHalfSize.y, -octantHalfSize.z), octantHalfSize); //top, back, right
	areas[6] = new BoundingBox(center + glm::vec3(-octantHalfSize.x, octantHalfSize.y, octantHalfSize.z), octantHalfSize); //top, front, left
	areas[7] = new BoundingBox(center + glm::vec3(octantHalfSize.x, octantHalfSize.y, octantHalfSize.z), octantHalfSize); //top, front, right

	//building all child nodes
	for(int i = 0; i < 8; i++) {
		std::vector<GameObject*> octantObjects; //keep track of the objects to pass to the child node

		for(unsigned int j = 0; j < _objects.size(); j++) {
			BoundingBox* objectBounds = _objects[j]->getBoundingBox();
			if(objectBounds == nullptr) continue; //skip, if the object has no collider

			//if the child node can hold the object, add it to our list
			if(BoundingBox::contains(areas[i], objectBounds)) {
				octantObjects.push_back(_objects[j]);
			}
		}

		if(octantObjects.size() > 0) {
			//tell the node that we own children
			if(!_hasChildren) _hasChildren = true;

			//delete added objects from object list
			for(unsigned j = 0; j < octantObjects.size(); j++) {
				_objects.erase(std::remove(_objects.begin(), _objects.end(), octantObjects[j]), _objects.end()); //remove the objects from the object list that have been added to an octant
			}

			_childNodes[i] = new Octree(_depth + 1, this);
			_childNodes[i]->buildTree(areas[i], octantObjects); //recursively build the children nodes
		} else {
			_childNodes[i] = nullptr;
			delete areas[i]; //not needed anymore, so free memory
		}
	}
}

void Octree::trashTree() {
	//remove the entire tree (pls no memory leaks)
	_destructOctree();
}

void Octree::updateNodes() {
	//handle deathtimer for this nodebranch
	if(_objects.size() == 0) {
		if(!_hasChildren) {
			if(_lifetime == -1) { //start lifetime countdown
				_lifetime = _maxLifetime;
			} else if(_lifetime > 0) { //update the countdown
				_lifetime--;
			}
		}
	} else {
		if(_lifetime != -1) { //reset timer, since this node owns objects now
			if(_maxLifetime <= 64) { //hardcap of about 1s
				_maxLifetime *= 2; //give it more time to live since it might be used often
				_lifetime = -1; //stop the timer
			}
		}
	}

	//recursively update child nodes
	for(int i = 0; i < 8; i++) {
		if(_childNodes[i] == nullptr) continue;

		_childNodes[i]->updateNodes();
	}

	//check for where the objects fit in the tree
	for(unsigned int i = 0; i < _objects.size(); i++) {
		Octree* current = this;
		GameObject* currentObject = _objects[i];

		if(currentObject->getBoundingBox() == nullptr) {
			_objects.erase(std::remove(_objects.begin(), _objects.end(), currentObject), _objects.end()); //remove the objects from the object list that have been added to an octant
			continue;
		}

		//look for the highest up node that can store the object
		while(!BoundingBox::contains(current->_bounds, currentObject->getBoundingBox())) {
			if(current->_parentNode != nullptr) current = current->_parentNode;
			else break; //stop at the root
		}

		_objects.erase(std::remove(_objects.begin(), _objects.end(), currentObject), _objects.end()); //remove the objects from the object list that have been added to an octant
		current->_insert(currentObject);
	}


	//update node state
	int childCount = 0;

	//remove any dead branches from the tree
	for(int i = 0; i < 8; i++) {
		if(_childNodes[i] == nullptr) continue;

		if(_childNodes[i]->_lifetime == 0) {
			//remove and cleanup the inactive node
			delete _childNodes[i];
			_childNodes[i] = nullptr;
		} else {
			childCount++;
		}
	}

	//update the state of the children
	if(childCount == 0) _hasChildren = false;
	else _hasChildren = true;
}

void Octree::clearObjects() {
	_objects.clear();

	if(_depth >= _TOTAL_DEPTH) return;

	for(int i = 0; i < 8; i++) {
		if(_childNodes[i] != nullptr) _childNodes[i]->clearObjects();
	}
}

void Octree::checkCollisions() {
	//only execute for the root node
	unsigned int objectCount = _objects.size();

	//execute collision detection for at least 2 objects in the object list
	if(objectCount > 1) {
		for(unsigned int i = 0; i < objectCount; i++) {
			BoundingBox* one = _objects[i]->getBoundingBox(); //no need to get collider every time in the other for loop
			if(one == nullptr) continue;

			//avoid checking collisons twice by assigning i to j as starting index
			for(unsigned j = i; j < objectCount; j++) {
				if(j == i) continue; //avoid checking against itself

				BoundingBox* other = _objects[j]->getBoundingBox();
				if(other == nullptr) continue;

				TestLog::COLLISION_CHECKS++;

				//collision detection calculation
				if(one->collidesWith(other)) {
					//notify behaviour about the collision
					_objects[i]->getBehaviour()->onCollision(other); //only resolve one cube with this setup
					//_objects[j]->getBehaviour()->onCollision(one);

					std::cout << "collision between " + _objects[i]->getName() + " and " + _objects[j]->getName() << std::endl;
					TestLog::COLLISIONS++;
				}
			}
		}
	}

	if(_depth >= _TOTAL_DEPTH) return; //when no children nodes are left

	//check collisions in children
	for(int i = 0; i < 8; i++) {
		if(_childNodes[i] != nullptr) _childNodes[i]->_checkCollisions(_objects); //otherwise forward the vector pointer from the parameter to the children
	}
}

void Octree::render(const glm::mat4 & pModelMatrix, const glm::mat4 & pViewMatrix, const glm::mat4 & pProjectionMatrix) {
	if(_octantRenderer != nullptr) {
		if(_objects.size() <= 0) _octantRenderer->setLineColor(glm::vec4(0.0f, 0.0f, 0.5f, 0.5f)); //blue, when there are no objects are in the octant
		else _octantRenderer->setLineColor(glm::vec4(0.5f, 0.0f, 0.0f, 1.0f)); //purple, when there are objectics in the octant

		//render self
		_octantRenderer->render(pModelMatrix, pViewMatrix, pProjectionMatrix);
	}

	if(_depth >= _TOTAL_DEPTH) return;

	//render children
	for(int i = 0; i < 8; i++) {
		if(_childNodes[i] != nullptr) _childNodes[i]->render(pModelMatrix, pViewMatrix, pProjectionMatrix);
	}
}

void Octree::_checkCollisions(std::vector<GameObject*> parentObjects) {
	unsigned int objectCount = _objects.size();
	unsigned int parentObjectCount = parentObjects.size();

	//execute collision detection for at least 2 objects in the object list or for 1 object each in the object list and the root node list
	if(objectCount > 1 || (objectCount > 0 && parentObjectCount > 0)) {
		for(unsigned i = 0; i < objectCount; i++) {
			BoundingBox* one = _objects[i]->getBoundingBox(); //no need to get collider every time in the other for loop
			if(one == nullptr) continue;

			//avoid checking collisons twice by assigning i to j as starting index
			for(unsigned j = i; j < objectCount; j++) {
				if(j == i) continue; //avoid checking against itself

				BoundingBox* other = _objects[j]->getBoundingBox();
				if(other == nullptr) continue;

				TestLog::COLLISION_CHECKS++;


				//collision detection calculation
				if(one->collidesWith(other)) {
					//notify behaviour about the collision
					_objects[i]->getBehaviour()->onCollision(other); //only resolve one cube with this setup
					//_objects[j]->getBehaviour()->onCollision(one);

					std::cout << "collision between " + _objects[i]->getName() + " and " + _objects[j]->getName() << std::endl;
					TestLog::COLLISIONS++;
				}
			}

			//check for collisions with the objects in the root node
			for(unsigned k = 0; k < parentObjectCount; k++) {

				BoundingBox* other = parentObjects[k]->getBoundingBox();
				if(other == nullptr) continue;

				TestLog::COLLISION_CHECKS++;

				//collision detection calculation
				if(one->collidesWith(other)) {
					//notify behaviour about the collision
					_objects[i]->getBehaviour()->onCollision(other); //only resolve one cube with this setup
					//parentObjects[k]->getBehaviour()->onCollision(one);

					std::cout << "collision between " + _objects[i]->getName() + " and " + parentObjects[k]->getName() << std::endl;
					TestLog::COLLISIONS++;
				}
			}
		}
	}

	if(_depth >= _TOTAL_DEPTH) return; //termiante recursion when no children nodes are left

	//merge parentobject list with current object list
	parentObjects.insert(parentObjects.end(), _objects.begin(), _objects.end()); //append the object list of this node to the parent objects list

	//check collisions in children
	for(int i = 0; i < 8; i++) {
		if(_childNodes[i] != nullptr) _childNodes[i]->_checkCollisions(parentObjects); //forward the merged vector by value (copy) to all children nodes
	}
}

void Octree::_initOctree(int depth) {
	if(depth <= 0) return; //stop constructing

	//new octant areas for the child nodes
	BoundingBox* areas[8];

	glm::vec3 octantHalfSize = _bounds->getHalfSize() * 0.5f; //half the halfsize to get halfsize for the new octant
	glm::vec3 center = _bounds->getCenter();

	//define new bounds for new areas
	areas[0] = new BoundingBox(center + glm::vec3(-octantHalfSize.x, -octantHalfSize.y, -octantHalfSize.z), octantHalfSize); //bottom, back, left
	areas[1] = new BoundingBox(center + glm::vec3(octantHalfSize.x, -octantHalfSize.y, -octantHalfSize.z), octantHalfSize); //bottom, back, right
	areas[2] = new BoundingBox(center + glm::vec3(-octantHalfSize.x, -octantHalfSize.y, octantHalfSize.z), octantHalfSize); //bottom, front, left
	areas[3] = new BoundingBox(center + glm::vec3(octantHalfSize.x, -octantHalfSize.y, octantHalfSize.z), octantHalfSize); //bottom, front, right
	areas[4] = new BoundingBox(center + glm::vec3(-octantHalfSize.x, octantHalfSize.y, -octantHalfSize.z), octantHalfSize); //top, back, left
	areas[5] = new BoundingBox(center + glm::vec3(octantHalfSize.x, octantHalfSize.y, -octantHalfSize.z), octantHalfSize); //top, back, right
	areas[6] = new BoundingBox(center + glm::vec3(-octantHalfSize.x, octantHalfSize.y, octantHalfSize.z), octantHalfSize); //top, front, left
	areas[7] = new BoundingBox(center + glm::vec3(octantHalfSize.x, octantHalfSize.y, octantHalfSize.z), octantHalfSize); //top, front, right

	//recursively create new nodes and tell them the new remaining depth
	for(int i = 0; i < 8; i++) {
		_childNodes[i] = new Octree(areas[i], depth - 1, this);
	}
}

void Octree::_insert(GameObject * movedObject) {
	if(_objects.size() < _NODE_TRESHOLD || _depth >= _TOTAL_DEPTH) {
		//if the subdivision threshold isnt reached yet or if the tree doesnt allow more depth, just add the object to the list
		_objects.push_back(movedObject);
		return;
	}

	//new octant areas for the potential child nodes
	BoundingBox* areas[8];

	glm::vec3 octantHalfSize = _bounds->getHalfSize() * 0.5f; //half the halfsize to get halfsize for the new octant
	glm::vec3 center = _bounds->getCenter();

	//define new bounds for new areas, if no child does exist, otherwise just get the bounds from the child node
	areas[0] = (_childNodes[0] != nullptr) ? _childNodes[0]->_bounds : new BoundingBox(center + glm::vec3(-octantHalfSize.x, -octantHalfSize.y, -octantHalfSize.z), octantHalfSize); //bottom, back, left
	areas[1] = (_childNodes[1] != nullptr) ? _childNodes[1]->_bounds : new BoundingBox(center + glm::vec3(octantHalfSize.x, -octantHalfSize.y, -octantHalfSize.z), octantHalfSize); //bottom, back, right
	areas[2] = (_childNodes[2] != nullptr) ? _childNodes[2]->_bounds : new BoundingBox(center + glm::vec3(-octantHalfSize.x, -octantHalfSize.y, octantHalfSize.z), octantHalfSize); //bottom, front, left
	areas[3] = (_childNodes[3] != nullptr) ? _childNodes[3]->_bounds : new BoundingBox(center + glm::vec3(octantHalfSize.x, -octantHalfSize.y, octantHalfSize.z), octantHalfSize); //bottom, front, right
	areas[4] = (_childNodes[4] != nullptr) ? _childNodes[4]->_bounds : new BoundingBox(center + glm::vec3(-octantHalfSize.x, octantHalfSize.y, -octantHalfSize.z), octantHalfSize); //top, back, left
	areas[5] = (_childNodes[5] != nullptr) ? _childNodes[5]->_bounds : new BoundingBox(center + glm::vec3(octantHalfSize.x, octantHalfSize.y, -octantHalfSize.z), octantHalfSize); //top, back, right
	areas[6] = (_childNodes[6] != nullptr) ? _childNodes[6]->_bounds : new BoundingBox(center + glm::vec3(-octantHalfSize.x, octantHalfSize.y, octantHalfSize.z), octantHalfSize); //top, front, left
	areas[7] = (_childNodes[7] != nullptr) ? _childNodes[7]->_bounds : new BoundingBox(center + glm::vec3(octantHalfSize.x, octantHalfSize.y, octantHalfSize.z), octantHalfSize); //top, front, right

	BoundingBox* objectBounds = movedObject->getBoundingBox();

	//check if this boundary includes the object
	if(BoundingBox::contains(_bounds, objectBounds)) {
		bool foundNode = false;

		//check which or if an area can contain the object better
		for(int i = 0; i < 8; i++) {
			if(foundNode) break;

			if(BoundingBox::contains(areas[i], objectBounds)) {
				if(_childNodes[i] != nullptr) {
					_childNodes[i]->_insert(movedObject); //try to insert it in the child
				} else {
					//create new node and add object to its list by initializing the node
					_childNodes[i] = new Octree(_depth - 1, this);
					_childNodes[i]->_initNode(areas[i], movedObject);
				}

				foundNode = true;
			}
		}

		if(!foundNode) _objects.push_back(movedObject); //object couldnt be stored in any child, so store it here
	} else {
		//item is not in the boundary of this node (and most likely we are in the root node, so its fine to store it if there is no other possible location)
		_objects.push_back(movedObject);
	}
}

void Octree::_initNode(BoundingBox * bounds, GameObject * movedObject) {
	//init node for further use
	_objects.push_back(movedObject);
	_bounds = bounds;
	_octantRenderer = new LineRenderer(bounds, true);
	_octantRenderer->setLineColor(glm::vec4(0.0f, 0.0f, 0.5f, 0.5f)); //blue
}

void Octree::_destructOctree() {
	//cleanup
	delete _octantRenderer;
	delete _bounds;

	if(_depth >= _TOTAL_DEPTH) return;

	for(unsigned int i = 0; i < 8; i++) {
		delete _childNodes[i];
	}
}