#ifndef OBB_H
#define OBB_H

#include "BoundingBox.h"
#include <glm.hpp>

class GameObject;

class OBB : public BoundingBox {
	public:
		OBB(GameObject* pOwner, glm::vec3 pHalfSize);
		~OBB();
};

#endif