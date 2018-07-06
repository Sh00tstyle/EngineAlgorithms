#ifndef MOVINGBEHAVIOUR_HPP
#define MOVINGBEHAVIOUR_HPP

#include "mge/behaviours/AbstractBehaviour.hpp"
#include <glm.hpp>

class BoundingBox;

class MovingBehaviour : public AbstractBehaviour
{
	public:
		MovingBehaviour(glm::vec3 pDirection, float pSpeed, glm::vec3 pOctreeBounds, bool pRotate = false);
		virtual ~MovingBehaviour();

		virtual void update(float pStep);

		virtual void onCollision(BoundingBox* other);
	
	private:
		glm::vec3 _direction;
		float _speed;
		bool _rotate;

		glm::vec3 _octreeBounds;

		BoundingBox* _bounds;

		void _resolveOutOfBounds();

};

#endif // MOVINGBEHAVIOUR_HPP
