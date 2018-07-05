#ifndef AABB_H
#define AABB_H

#include "BoundingBox.h"
#include <glm.hpp>

class GameObject;

class AABB : public BoundingBox {
public:
	AABB(GameObject* pOwner, glm::vec3 pHalfSize);
	~AABB();
};

#endif