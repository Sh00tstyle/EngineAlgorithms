#include "mge/behaviours/AbstractBehaviour.hpp"
#include "mge/core/Light.hpp"
#include <SFML\Window.hpp>

/**
 * KeysBehaviour allows you to move an object using the keyboard in its own local space.
 * Left right turns, forward back moves.
 */
class LightControlBehaviour : public AbstractBehaviour
{
	public:
	    //move speed is in units per second, turnspeed in degrees per second
		LightControlBehaviour(Light* pLight, float pMoveSpeed = 5);
		virtual ~LightControlBehaviour();
		virtual void update( float pStep );

    private:
		Light* _light; //needed to change the light type in runtime
        float _moveSpeed;

		glm::mat4 _startTransform;
};