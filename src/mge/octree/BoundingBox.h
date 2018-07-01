#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <glm.hpp>

class GameObject;

class BoundingBox {

	public:
		BoundingBox(glm::vec3 pCenter, glm::vec3 pHalfSize); //for octree bounds
		BoundingBox(GameObject* pOwner, glm::vec3 pHalfSize); //for gameobject colliders
		~BoundingBox();

		//local space
		glm::vec3 getHalfSize();

		//world space
		glm::vec3 getCenter();
		glm::vec3 getMin();
		glm::vec3 getMax();

		bool contains(BoundingBox* other);

		//access to the gameobject (maybe not needed)
		GameObject* getOwner();

	private:
		glm::vec3 _center;
		glm::vec3 _halfSize;
		GameObject* _owner;

};

#endif