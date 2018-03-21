#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "glm.hpp"
#include "mge/core/GameObject.hpp"

enum LightType { DIRECTIONAL, POINT, SPOT }; //different types of light

/**
 * Exercise for the student: implement the Light class...
 * Imagine things like setLightType, setLightIntensity, setFalloffAngle etc, see the corresponding lectures
 */
class Light : public GameObject
{
	public:
		Light(LightType pLightType, glm::vec3 pLightColor, float pIntensity = 1.0f, float pAmbientContribution = 1.0f, float pConstantAttenuation = 1.0f, float pLinearAttenuation = 0.0f, 
			  float pQuadraticAttenuation = 0.0f, float pOuterConeAngle = 45.0f, float pInnerConeAngle = 25.0f, const std::string& aName = nullptr, const glm::vec3& aPosition = glm::vec3( 2.0f, 10.0f, 5.0f ));
		virtual ~Light();

        //override set parent to register/deregister light...
        virtual void _setWorldRecursively (World* pWorld) override;

		void setLightType(LightType type);
		LightType getLightType();

		void setLightColor(glm::vec3 color);
		glm::vec3 getLightColor();

		void setIntensity(float intensity);
		float getIntensity();

		void setAmbientContribution(float contribution);
		float getAmbientContribution();

		void setConstantAttenuation(float value);
		float getConstantAttenuation();

		void setLinearAttenuation(float value);
		float getLinearAttenuation();

		void setQuadraticAttenuation(float value);
		float getQuadraticAttenuation();

		void setOuterConeAngle(float angle);
		float getOuterConeAngle();

		void setInnerConeAngle(float angle);
		float getInnerConeAngle();
	private:
		LightType _lightType;

		glm::vec3 _lightColor;
		float _intensity;
		float _ambientContribution;

		float _constantAttenuation;
		float _linearAttenuation;
		float _quadraticAttenuation;

		float _outerConeAngle;
		float _innerConeAngle;
};

#endif // LIGHT_HPP