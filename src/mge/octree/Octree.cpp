#include <iostream>
#include <string>
#include "OctreeWorld.h"
#include "Octree.h"
#include "BoundingBox.h"
#include "mge\core/LineRenderer.hpp"
#include "mge/behaviours/MovingBehaviour.hpp"

int Octree::TOTAL_DEPTH = 0; //init static depth member

Octree::Octree(BoundingBox * pBounds, int pDepth, Octree * pParentNode) : _bounds(pBounds), _parentNode(pParentNode) {
	if(_parentNode == nullptr) TOTAL_DEPTH = pDepth; //store total depth to reconstruct octree later
	_depth = TOTAL_DEPTH - pDepth; //depth of the node itself

	_octantRenderer = new LineRenderer(_bounds, true);
	_octantRenderer->setLineColor(glm::vec4(0.0f, 0.0f, 0.5f, 0.5f)); //blue

	_initOctree(pDepth); //build octree for the first time, since it is the root node
}

Octree::~Octree() {
	_destructOctree(); //remove all child nodes if there are any
}

void Octree::addObject(GameObject * newObject) {
	_objects.push_back(newObject);
}

//returns true, if the node could store the object otherwise returns false
bool Octree::updateNodes(GameObject* gameObject) {
	if(_contains(gameObject->getLocalPosition())) {
		if(_depth < TOTAL_DEPTH) {
			//gameobject is in the current octant, but check for children
			for(int i = 0; i < 8; i++) {
				if(_childNodes[i]->updateNodes(gameObject)) {
					//object could be stored, so return true
					return true;
				}
			}
		}

		//child nodes can't hold the object or there are no child nodes left, so store it in this node and return true
		_objects.push_back(gameObject);
		return true;
	}

	//this octant doesnt store the object, so return false
	return false;
}

void Octree::clearObjects() {
	_objects.clear();

	if(_depth >= TOTAL_DEPTH) return;

	for(int i = 0; i < 8; i++) {
		_childNodes[i]->clearObjects();
	}
}

void Octree::checkCollisions() {
	//execute collision detection for at least 2 objects in the list
	if(_objects.size() > 1) {
		for(unsigned i = 0; i < _objects.size(); i++) {
			//avoid checking collisons twice for
			for(unsigned j = i; j < _objects.size(); j++) {
				if(j == i) continue; //avoid checking against itself

				//collision detection calculation
				if(_isColliding(_objects[i]->getBoundingBox(), _objects[j]->getBoundingBox())) {
					//register collisions in the behaviours
					_objects[i]->getMovingBehaviour()->onCollision(_objects[j]->getBoundingBox());
					//_objects[j]->getMovingBehaviour()->onCollision(_objects[i]->getBoundingBox());

					std::cout << "collision between " + _objects[i]->getName() + " and " + _objects[j]->getName() << std::endl;
				}
			}
		}
	}

	if(_depth >= TOTAL_DEPTH) return;

	//check collisions in children
	for(int i = 0; i < 8; i++) {
		_childNodes[i]->checkCollisions();
	}
}

void Octree::render(const glm::mat4 & pModelMatrix, const glm::mat4 & pViewMatrix, const glm::mat4 & pProjectionMatrix) {
	if(_objects.size() <= 0) _octantRenderer->setLineColor(glm::vec4(0.0f, 0.0f, 0.5f, 0.5f)); //blue, when there are no objects are in the octant
	else _octantRenderer->setLineColor(glm::vec4(0.5f, 0.0f, 0.0f, 1.0f)); //purple, when there are objectics in the octant

	//render self
	_octantRenderer->render(pModelMatrix, pViewMatrix, pProjectionMatrix);

	if(_depth >= TOTAL_DEPTH) return;

	//render children
	for(int i = 0; i < 8; i++) {
		_childNodes[i]->render(pModelMatrix, pViewMatrix, pProjectionMatrix);
	}
}

bool Octree::_contains(glm::vec3 otherPos) {
	//returns true, if the center of the other object is in the boundaries of the current octant
	glm::vec3 min = _bounds->getMin();
	glm::vec3 max = _bounds->getMax();

	return (otherPos.x > min.x &&
			otherPos.x < max.x &&
			otherPos.y > min.y &&
			otherPos.y < max.y &&
			otherPos.z > min.z &&
			otherPos.z < max.z);
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

void Octree::_destructOctree() {
	if(_depth >= TOTAL_DEPTH) return; //dont destruct if there is no depth left

	for(unsigned int i = 0; i < 8; i++) {
		delete _childNodes[i];
	}

	delete _octantRenderer;
}

bool Octree::_isColliding(BoundingBox * one, BoundingBox * other) {
	/**/
	//AABB vs AABB 
	glm::vec3 oneMin = one->getMin();
	glm::vec3 oneMax = one->getMax();

	glm::vec3 otherMin = other->getMin();
	glm::vec3 otherMax = other->getMax();

	return (oneMax.x > otherMin.x &&
			oneMin.x < otherMax.x &&
			oneMax.y > otherMin.y &&
			oneMin.y < otherMax.y &&
			oneMax.z > otherMin.z &&
			oneMin.z < otherMax.z);

	/**
	//OBB vs OBB 
	glm::vec3 oneCenter = one->getCenter(); // object's pos = collider center
	glm::mat4 oneTransform = one->getOwner()->getTransform(); // scaling for halfsize
	glm::vec3 otherCenter = other->getCenter();
	glm::mat4 otherTransform = other->getOwner()->getTransform();

	for(int a = 0; a < 3; a++) {
		glm::vec3 l = glm::vec3(oneTransform[a]); // one axis to project on
		float tl = std::abs(glm::dot(l, otherCenter) - glm::dot(l, oneCenter)); // center distance
		float ra = std::abs(glm::dot(l, glm::vec3(oneTransform[0]))) + std::abs(glm::dot(l, glm::vec3(oneTransform[1]))) + std::abs(glm::dot(l, glm::vec3(oneTransform[2])));
		float rb = std::abs(glm::dot(l, glm::vec3(otherTransform[0]))) + std::abs(glm::dot(l, glm::vec3(otherTransform[1]))) + std::abs(glm::dot(l, glm::vec3(otherTransform[2])));
		float penetration = (ra + rb) - tl;
		if(penetration <= 0) { // no overlap
			return false;
		}
	}

	for(int b = 0; b < 3; b++) {
		glm::vec3 l = glm::vec3(otherTransform[b]); // other axis to project on
		float tl = std::abs(glm::dot(l, otherCenter) - glm::dot(l, oneCenter)); // center distance
		float ra = std::abs(glm::dot(l, glm::vec3(oneTransform[0]))) + std::abs(glm::dot(l, glm::vec3(oneTransform[1]))) + std::abs(glm::dot(l, glm::vec3(oneTransform[2])));
		float rb = std::abs(glm::dot(l, glm::vec3(otherTransform[0]))) + std::abs(glm::dot(l, glm::vec3(otherTransform[1]))) + std::abs(glm::dot(l, glm::vec3(otherTransform[2])));
		float penetration = (ra + rb) - tl;
		if(penetration <= 0) { // no overlap
			return false;
		}
	}

	for(int a = 0; a < 3; a++) {
		glm::vec3 aAxis = glm::vec3(oneTransform[a]);
		for(int b = 0; b < 3; b++) {
			glm::vec3 bAxis = glm::vec3(otherTransform[b]);
			if(aAxis != bAxis) {
				glm::vec3 l = glm::cross(aAxis, bAxis); // has flaw when axis are same, result in (0,0,0), solved by if
				float tl = std::abs(glm::dot(l, otherCenter) - glm::dot(l, oneCenter)); // center distance
				float ra = std::abs(glm::dot(l, glm::vec3(oneTransform[0]))) + std::abs(glm::dot(l, glm::vec3(oneTransform[1]))) + std::abs(glm::dot(l, glm::vec3(oneTransform[2])));
				float rb = std::abs(glm::dot(l, glm::vec3(otherTransform[0]))) + std::abs(glm::dot(l, glm::vec3(otherTransform[1]))) + std::abs(glm::dot(l, glm::vec3(otherTransform[2])));
				float penetration = (ra + rb) - tl;
				if(penetration <= 0) { // no overlap
					return false;
				}
			}
		}
	}

	return true;
	/**/
}
