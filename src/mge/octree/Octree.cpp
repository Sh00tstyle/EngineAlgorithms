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

Octree::Octree(BoundingBox * pBounds, int pDepth, Octree * pParentNode) : _bounds(pBounds), _parentNode(pParentNode), _dynamicObjects() {
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

Octree::Octree(int pDepth, Octree* pParentNode) : _bounds(nullptr), _parentNode(pParentNode), _octantRenderer(nullptr), _dynamicObjects() {
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
	_dynamicObjects.push_back(newObject);
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
	_dynamicObjects = objects; //at this point the static and dynamic objects are not split/filtered out yet
	_bounds = bounds;
	_octantRenderer = new LineRenderer(bounds, true);
	_octantRenderer->setLineColor(glm::vec4(0.0f, 0.0f, 0.5f, 0.5f)); //blue

	if(_dynamicObjects.size() <= _NODE_TRESHOLD || _depth >= _TOTAL_DEPTH) return; //terminate recursion, if we reached a leaf node

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

		for(unsigned int j = 0; j < _dynamicObjects.size(); j++) {
			BoundingBox* objectBounds = _dynamicObjects[j]->getBoundingBox();
			if(objectBounds == nullptr) continue; //skip, if the object has no collider

			//if the child node can hold the object, add it to our list
			if(BoundingBox::contains(areas[i], objectBounds)) {
				octantObjects.push_back(_dynamicObjects[j]);
			}
		}

		if(octantObjects.size() > 0) {
			//tell the node that we own children
			if(!_hasChildren) _hasChildren = true;

			//delete added objects from object list
			for(unsigned j = 0; j < octantObjects.size(); j++) {
				_dynamicObjects.erase(std::remove(_dynamicObjects.begin(), _dynamicObjects.end(), octantObjects[j]), _dynamicObjects.end()); //remove the objects from the object list that have been added to an octant
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
	if((_dynamicObjects.size() + _staticObjects.size()) == 0) {
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

	//check for where the objects fit in the tree (NOTE: Only doing this for dynamic objects, since statics never move and always remain in their branch)
	for(unsigned int i = 0; i < _dynamicObjects.size(); i++) {
		Octree* current = this;
		GameObject* currentObject = _dynamicObjects[i];

		if(currentObject->getBoundingBox() == nullptr) {
			_dynamicObjects.erase(std::remove(_dynamicObjects.begin(), _dynamicObjects.end(), currentObject), _dynamicObjects.end()); //remove the objects from the object list that have been added to an octant
			continue;
		}

		//look for the highest up node that can store the object
		while(!BoundingBox::contains(current->_bounds, currentObject->getBoundingBox())) {
			if(current->_parentNode != nullptr) current = current->_parentNode;
			else break; //stop at the root
		}

		_dynamicObjects.erase(std::remove(_dynamicObjects.begin(), _dynamicObjects.end(), currentObject), _dynamicObjects.end()); //remove the objects from the object list that have been added to an octant
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
	_dynamicObjects.clear();

	if(_depth >= _TOTAL_DEPTH) return;

	for(int i = 0; i < 8; i++) {
		if(_childNodes[i] != nullptr) _childNodes[i]->clearObjects();
	}
}

void Octree::checkCollisions() {
	//only execute for the root node

	//execute collision detection for at least 2 objects in the object list
	if((_dynamicObjects.size() + _staticObjects.size()) > 1) {
		for(unsigned int i = 0; i < _dynamicObjects.size(); i++) {
			BoundingBox* one = _dynamicObjects[i]->getBoundingBox(); //no need to get collider every time in the other for loop
			if(one == nullptr) continue;

			//check against other dynamic objects
			for(unsigned int j = i; j < _dynamicObjects.size(); j++) {
				if(j == i) continue; //avoid checking against itself

				BoundingBox* other = _dynamicObjects[j]->getBoundingBox();
				if(other == nullptr) continue;

				TestLog::COLLISION_CHECKS++;

				//collision detection calculation
				if(one->collidesWith(other)) {
					//notify behaviour about the collision
					_dynamicObjects[i]->getBehaviour()->onCollision(other); //only resolve one cube with this setup
					//_dynamicObjects[j]->getBehaviour()->onCollision(one);

					std::cout << "collision between " + _dynamicObjects[i]->getName() + " and " + _dynamicObjects[j]->getName() << std::endl;
					TestLog::COLLISIONS++;
				}
			}

			//check against static objects
			for(unsigned int j = 0; j < _staticObjects.size(); j++) {
				BoundingBox* other = _staticObjects[j]->getBoundingBox();
				if(other == nullptr) continue;

				TestLog::COLLISION_CHECKS++;

				//collision detection calculation
				if(one->collidesWith(other)) {
					//notify behaviour about the collision
					_dynamicObjects[i]->getBehaviour()->onCollision(other); //only resolve one cube with this setup
					//_staticObjects[j]->getBehaviour()->onCollision(one);

					std::cout << "collision between " + _dynamicObjects[i]->getName() + " and " + _staticObjects[j]->getName() << std::endl;
					TestLog::COLLISIONS++;
				}
			}
		}
	}

	if(_depth >= _TOTAL_DEPTH) return; //when no children nodes are left

	//check collisions in children
	for(int i = 0; i < 8; i++) {
		if(_childNodes[i] != nullptr) _childNodes[i]->_checkCollisions(_dynamicObjects); //forward the dynamic objects to the children
	}
}

void Octree::render(const glm::mat4 & pModelMatrix, const glm::mat4 & pViewMatrix, const glm::mat4 & pProjectionMatrix) {
	if(_octantRenderer != nullptr) {
		if((_dynamicObjects.size() + _staticObjects.size()) <= 0) _octantRenderer->setLineColor(glm::vec4(0.0f, 0.0f, 0.5f, 0.5f)); //blue, when there are no objects are in the octant
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
	//execute collision detection for at least 2 objects in the object list or for 1 object each in the object list and the root node list
	if((_dynamicObjects.size() + _staticObjects.size() + parentObjects.size()) > 1) {
		for(unsigned i = 0; i < _dynamicObjects.size(); i++) {
			BoundingBox* one = _dynamicObjects[i]->getBoundingBox(); //no need to get collider every time in the other for loop
			if(one == nullptr) continue;

			//check against other dynamic objects
			for(unsigned j = i; j < _dynamicObjects.size(); j++) {
				if(j == i) continue; //avoid checking against itself

				BoundingBox* other = _dynamicObjects[j]->getBoundingBox();
				if(other == nullptr) continue;

				TestLog::COLLISION_CHECKS++;

				//collision detection calculation
				if(one->collidesWith(other)) {
					//notify behaviour about the collision
					_dynamicObjects[i]->getBehaviour()->onCollision(other); //only resolve one cube with this setup
					//_dynamicObjects[j]->getBehaviour()->onCollision(one);

					std::cout << "collision between " + _dynamicObjects[i]->getName() + " and " + _dynamicObjects[j]->getName() << std::endl;
					TestLog::COLLISIONS++;
				}
			}

			//check against objects in the root node
			for(unsigned j = 0; j < parentObjects.size(); j++) {

				BoundingBox* other = parentObjects[j]->getBoundingBox();
				if(other == nullptr) continue;

				TestLog::COLLISION_CHECKS++;

				//collision detection calculation
				if(one->collidesWith(other)) {
					//notify behaviour about the collision
					_dynamicObjects[i]->getBehaviour()->onCollision(other); //only resolve one cube with this setup
					//parentObjects[j]->getBehaviour()->onCollision(one);

					std::cout << "collision between " + _dynamicObjects[i]->getName() + " and " + parentObjects[j]->getName() << std::endl;
					TestLog::COLLISIONS++;
				}
			}

			//check against static objects
			for(unsigned int j = 0; j < _staticObjects.size(); j++) {
				BoundingBox* other = _staticObjects[j]->getBoundingBox();
				if(other == nullptr) continue;

				TestLog::COLLISION_CHECKS++;

				//collision detection calculation
				if(one->collidesWith(other)) {
					//notify behaviour about the collision
					_dynamicObjects[i]->getBehaviour()->onCollision(other); //only resolve one cube with this setup
					//_staticObjects[j]->getBehaviour()->onCollision(one);

					std::cout << "collision between " + _dynamicObjects[i]->getName() + " and " + _staticObjects[j]->getName() << std::endl;
					TestLog::COLLISIONS++;
				}
			}
		}
	}

	if(_depth >= _TOTAL_DEPTH) return; //termiante recursion when no children nodes are left

	//merge parentobject list with current object list
	parentObjects.insert(parentObjects.end(), _dynamicObjects.begin(), _dynamicObjects.end()); //append the object list of this node to the parent objects list

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
	//NOTE: insert can only be called for dynamic objects
	if((_dynamicObjects.size() + _staticObjects.size()) < _NODE_TRESHOLD || _depth >= _TOTAL_DEPTH) {
		//if the subdivision threshold isnt reached yet or if the tree doesnt allow more depth, just add the object to the list
		_dynamicObjects.push_back(movedObject);
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

		if(!foundNode) _dynamicObjects.push_back(movedObject); //object couldnt be stored in any child, so store it here
	} else {
		//item is not in the boundary of this node (and most likely we are in the root node, so its fine to store it if there is no other possible location)
		_dynamicObjects.push_back(movedObject);
	}
}

void Octree::_initNode(BoundingBox * bounds, GameObject * movedObject) {
	//init node for further use
	_dynamicObjects.push_back(movedObject);
	_bounds = bounds;
	_octantRenderer = new LineRenderer(bounds, true);
	_octantRenderer->setLineColor(glm::vec4(0.0f, 0.0f, 0.5f, 0.5f)); //blue
}

void Octree::filterStatics() {
	//filter the object list for dynamic and static objects and store them accordingly
	for(unsigned int i = 0; i < _dynamicObjects.size(); i++) {
		if(_dynamicObjects[i]->isStatic()) {
			_staticObjects.push_back(_dynamicObjects[i]);
		}
	}

	//remove objects that are in both lists from the dynamic list to avoid duplicates
	for(unsigned int i = 0; i < _staticObjects.size(); i++) {
		_dynamicObjects.erase(std::remove(_dynamicObjects.begin(), _dynamicObjects.end(), _staticObjects[i]), _dynamicObjects.end());
	}

	if(_depth >= _TOTAL_DEPTH) return; //terminate recursion

	//recursively filter the childnodes for static objects
	for(int i = 0; i < 8; i++) {
		if(_childNodes[i] != nullptr) _childNodes[i]->filterStatics();
	}
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