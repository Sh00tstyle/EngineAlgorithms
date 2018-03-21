#include "mge/behaviours/AbstractBehaviour.hpp"
#include <SFML/System/Vector2.hpp>
#include "glm.hpp"

class CameraOrbitBehaviour : public AbstractBehaviour {
	public:
	CameraOrbitBehaviour(float pDistance, float pMinAngle, float pMaxAngle, float pRotationSpeed, GameObject* pTargetObject);
	virtual ~CameraOrbitBehaviour();

	virtual void update(float pStep);

	private:
	float _distance;
	float _minAngle;
	float _maxAngle;
	float _rotationSpeed;
	GameObject* _targetObject;

	sf::Vector2i _prevMousePos;

	glm::mat4 _rotationYMatrix;
	glm::mat4 _rotationXMatrix;

	float _xAngle; //Range: -1.8 to 1.8 (0.1 = 10°)
};