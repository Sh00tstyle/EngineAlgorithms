#ifndef OBB_H
#define OBB_H

#include "BoundingBox.h"
#include <glm.hpp>

class GameObject;
class AABB;

class OBB : public BoundingBox {
	public:
		OBB(GameObject* pOwner, glm::vec3 pHalfSize);
		~OBB();

		virtual bool collidesWith(BoundingBox* other);
		virtual bool collidesWith(AABB* other);
		virtual bool collidesWith(OBB* other);
};

#endif