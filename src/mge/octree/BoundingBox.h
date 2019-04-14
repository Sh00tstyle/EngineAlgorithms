#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <glm.hpp>

class GameObject;
class LineRenderer;
class AABB;
class OBB;

class BoundingBox {

	public:
		BoundingBox(glm::vec3 pCenter, glm::vec3 pHalfSize, unsigned int pType = 0); //for octree bounds
		BoundingBox(GameObject* pOwner, glm::vec3 pHalfSize, unsigned int pType = 0); //for gameobject colliders
		~BoundingBox();

		//collision detection calculations
		static bool isColliding(AABB* one, AABB* other);
		static bool isColliding(AABB* one, OBB* other);
		static bool isColliding(OBB* one, OBB* other);

		//fit test
		static bool contains(BoundingBox* bounds, BoundingBox* boxToCheck);

		//warpper to decide whether to use double dispatching or not
		bool checkCollision(BoundingBox* other);

		//double dispatching
		virtual bool collidesWith(BoundingBox* other);
		virtual bool collidesWith(AABB* other);
		virtual bool collidesWith(OBB* other);

		//switch collision
		virtual bool collidesWithSwitch(BoundingBox* other);

		//local space
		glm::vec3 getHalfSize();
		void setHalfSize(glm::vec3 newHalfSize);

		//world space
		glm::vec3 getCenter();
		glm::vec3 getMin();
		glm::vec3 getMax();

		//access to the gameobject (maybe not needed)
		GameObject* getOwner();

		//access to the collider type
		unsigned int getType();

	private:
		static const glm::mat4 _AABB_AXES; //uniform matrix to replace the object transform for AABB

		unsigned int _type; //0 = BoundingBox, 1 = AABB, 2 = OBB
		glm::vec3 _center;
		glm::vec3 _halfSize;
		GameObject* _owner;


};

#endif