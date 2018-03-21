#include "glm.hpp"

#include "LitMaterial.h"
#include "mge/config.hpp"
#include "mge/core/GameObject.hpp"
#include "mge/core/Mesh.hpp"
#include "mge/core/ShaderProgram.hpp"
#include "mge/core/Light.hpp"
#include "mge/core/World.hpp"
#include "mge/core/Camera.hpp"

ShaderProgram* LitMaterial::_shader = NULL;

LitMaterial::LitMaterial(glm::vec3 pAmbientColor, glm::vec3 pDiffuseColor, glm::vec3 pSpecularColor, float pShininess) :
	_ambientColor(pAmbientColor), _diffuseColor(pDiffuseColor), _specularColor(pSpecularColor), _shininess(pShininess) {
	//every time we create an instance of colormaterial we check if the corresponding shader has already been loaded
	_lazyInitializeShader();
}

LitMaterial::~LitMaterial() {
	//destructor
}

void LitMaterial::_lazyInitializeShader() {
	//this shader contains everything the material can do (it can render something in 3d using a single color)
	if(!_shader) {
		_shader = new ShaderProgram();
		_shader->addShader(GL_VERTEX_SHADER, config::MGE_SHADER_PATH + "lit.vs"); //Assignment 3.1, loading lit vertex shader
		_shader->addShader(GL_FRAGMENT_SHADER, config::MGE_SHADER_PATH + "lit.fs"); //Assignment 3.1, loading lit fragment shader
		_shader->finalize();
	}
}

void LitMaterial::setAmbientColor(glm::vec3 pAmbientColor) {
	_ambientColor = pAmbientColor;
}

void LitMaterial::setDiffuseColor(glm::vec3 pDiffuseColor) {
	_diffuseColor = pDiffuseColor;
}

void LitMaterial::setSpecularColor(glm::vec3 pSpecularColor) {
	_specularColor = pSpecularColor;
}

void LitMaterial::setShininess(float pShininess) {
	_shininess = pShininess;
}

void LitMaterial::render(World* pWorld, Mesh* pMesh, const glm::mat4& pModelMatrix, const glm::mat4& pViewMatrix, const glm::mat4& pProjectionMatrix) {
	_shader->use();

	Camera* mainCam = pWorld->getMainCamera(); //needed for specular eye position
	int lightAmount = pWorld->getLightCount();

	//passing in light amount
	glUniform1i(_shader->getUniformLocation("lightAmount"), lightAmount); //passing in the amount of light we have

	//iterate through the lights and add them to the uniform array
	for(int i = 0; i < lightAmount; i++) {
		Light* currentLight = pWorld->getLightAt(i);
		std::string lightString = "lights[" + std::to_string(i) + "]."; //"path" to the right array element

		glUniform1i(_shader->getUniformLocation(lightString + "lightType"), (int)currentLight->getLightType()); //passing in the light type as an int

		glUniform1f(_shader->getUniformLocation(lightString + "ambientContribution"), std::cos(currentLight->getInnerConeAngle()));

		glUniform3fv(_shader->getUniformLocation(lightString + "lightForward"), 1, glm::value_ptr(currentLight->getTransform()[2])); //third row of the lights transform, represents local forward vector
		glUniform3fv(_shader->getUniformLocation(lightString + "lightPosition"), 1, glm::value_ptr(currentLight->getLocalPosition())); //might need to be changed to world pos

		glUniform1f(_shader->getUniformLocation(lightString + "constantAttenuation"), currentLight->getConstantAttenuation());
		glUniform1f(_shader->getUniformLocation(lightString + "linearAttenuation"), currentLight->getLinearAttenuation());
		glUniform1f(_shader->getUniformLocation(lightString + "quadraticAttenuation"), currentLight->getQuadraticAttenuation());

		glUniform1f(_shader->getUniformLocation(lightString + "outerConeCos"), std::cos(currentLight->getOuterConeAngle()));
		glUniform1f(_shader->getUniformLocation(lightString + "innerConeCos"), std::cos(currentLight->getInnerConeAngle()));

		glUniform3fv(_shader->getUniformLocation(lightString + "lightColor"), 1, glm::value_ptr(currentLight->getLightColor() * currentLight->getIntensity())); //applying intensity, before passing it
	}

	//passing all material properties to the shader
	glUniform3fv(_shader->getUniformLocation("ambientColor"), 1, glm::value_ptr(_ambientColor)); //applying contribution, before passing it
	glUniform3fv(_shader->getUniformLocation("diffuseColor"), 1, glm::value_ptr(_diffuseColor));
	glUniform3fv(_shader->getUniformLocation("specularColor"), 1, glm::value_ptr(_specularColor));
	glUniform1f(_shader->getUniformLocation("shininess"), _shininess);

	//passing in camera position
	glUniform3fv(_shader->getUniformLocation("eyePosition"), 1, glm::value_ptr(mainCam->getWorldPosition())); //might need to be changed to world pos

	//pass in all MVP matrices separately
	glUniformMatrix4fv(_shader->getUniformLocation("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(pProjectionMatrix));
	glUniformMatrix4fv(_shader->getUniformLocation("viewMatrix"), 1, GL_FALSE, glm::value_ptr(pViewMatrix));
	glUniformMatrix4fv(_shader->getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(pModelMatrix));

	//now inform mesh of where to stream its data
	pMesh->streamToOpenGL(
		_shader->getAttribLocation("vertex"),
		_shader->getAttribLocation("normal"),
		_shader->getAttribLocation("uv")
	);
}
