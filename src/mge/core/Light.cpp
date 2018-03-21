#include "mge/core/Light.hpp"
#include "mge/core/World.hpp"
#include "mge/materials/ColorMaterial.hpp"

Light::Light(LightType pLightType, glm::vec3 pLightColor, float pIntensity, float pAmbientContribution, float pConstantAttenuation, 
			 float pLinearAttenuation, float pQuadraticAttenuation, float pOuterConeAngle, float pInnerConeAngle, const std::string& pName, const glm::vec3& pPosition) : GameObject(pName, pPosition),
	_lightType(pLightType), _lightColor(pLightColor), _intensity(pIntensity), _ambientContribution(pAmbientContribution), _constantAttenuation(pConstantAttenuation), 
	_linearAttenuation(pLinearAttenuation), _quadraticAttenuation(pQuadraticAttenuation), _outerConeAngle(pOuterConeAngle), _innerConeAngle(pInnerConeAngle) {
	//constructor

	AbstractMaterial* lightColorMat = new ColorMaterial(pLightColor); //setting to color of the object to the same color as the light
	setMaterial(lightColorMat);
}

Light::~Light() {
	//destructor
}

//Override setWorldRecursively to make sure we are registered
//to the world we are a part of.
void Light::_setWorldRecursively(World* pWorld) {

	//store the previous and new world before and after storing the given world
	World* previousWorld = _world;
	GameObject::_setWorldRecursively(pWorld);
	World* newWorld = _world;

	//check whether we need to register or unregister
	if(previousWorld != nullptr) previousWorld->unregisterLight(this);
	if(newWorld != nullptr) newWorld->registerLight(this);
}

void Light::setLightType(LightType type) {
	_lightType = type;
}

LightType Light::getLightType() {
	return _lightType;
}

void Light::setLightColor(glm::vec3 color) {
	_lightColor = color;
}

glm::vec3 Light::getLightColor() {
	return _lightColor;
}

void Light::setIntensity(float intensity) {
	_intensity = intensity;
}

float Light::getIntensity() {
	return _intensity;
}

void Light::setAmbientContribution(float contribution) {
	_ambientContribution = contribution;
}

float Light::getAmbientContribution() {
	return _ambientContribution;
}

void Light::setConstantAttenuation(float value) {
	_constantAttenuation = value;
}

float Light::getConstantAttenuation() {
	return _constantAttenuation;
}

void Light::setLinearAttenuation(float value) {
	_linearAttenuation = value;
}

float Light::getLinearAttenuation() {
	return _linearAttenuation;
}

void Light::setQuadraticAttenuation(float value) {
	_quadraticAttenuation = value;
}

float Light::getQuadraticAttenuation() {
	return _quadraticAttenuation;
}

void Light::setOuterConeAngle(float angle) {
	_outerConeAngle = angle;
}

float Light::getOuterConeAngle() {
	return _outerConeAngle;
}

void Light::setInnerConeAngle(float angle) {
	_innerConeAngle = angle;
}

float Light::getInnerConeAngle() {
	return _innerConeAngle;
}