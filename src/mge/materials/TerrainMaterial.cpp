#include "glm.hpp"

#include "TerrainMaterial.h"
#include "mge/core/Texture.hpp"
#include "mge/core/Light.hpp"
#include "mge/core/Camera.hpp"
#include "mge/core/World.hpp"
#include "mge/core/Mesh.hpp"
#include "mge/core/GameObject.hpp"
#include "mge/core/ShaderProgram.hpp"
#include "mge/config.hpp"

ShaderProgram* TerrainMaterial::_shader = NULL;

GLint TerrainMaterial::_aVertex = 0;
GLint TerrainMaterial::_aNormal = 0;
GLint TerrainMaterial::_aUV = 0;

TerrainMaterial::TerrainMaterial(glm::vec3 pAmbientColor, glm::vec3 pSpecularColor, float pShininess, Texture * pDiffuseTexture1, Texture * pDiffuseTexture2, Texture * pDiffuseTexture3, 
								 Texture * pDiffuseTexture4, Texture* pHeightMapTexture, Texture* pSplatMapTexture):
	_ambientColor(pAmbientColor), _specularColor(pSpecularColor), _shininess(pShininess), _diffuseTexture1(pDiffuseTexture1), _diffuseTexture2(pDiffuseTexture2), _diffuseTexture3(pDiffuseTexture3), 
	_diffuseTexture4(pDiffuseTexture4), _heightMapTexture(pHeightMapTexture), _splatMapTexture(pSplatMapTexture) {
    _lazyInitializeShader();
}

TerrainMaterial::~TerrainMaterial() {}

void TerrainMaterial::_lazyInitializeShader() {
    if (!_shader) {
        _shader = new ShaderProgram();
        _shader->addShader(GL_VERTEX_SHADER, config::MGE_SHADER_PATH+"terrain.vs");
        _shader->addShader(GL_FRAGMENT_SHADER, config::MGE_SHADER_PATH+"terrain.fs");
        _shader->finalize();

        //cache all the attribute indexes
        _aVertex = _shader->getAttribLocation("vertex");
        _aNormal = _shader->getAttribLocation("normal");
        _aUV =     _shader->getAttribLocation("uv");
    }
}

void TerrainMaterial::setAmbientColor(glm::vec3 color) {
	_ambientColor = color;
}

void TerrainMaterial::setSpecularColor(glm::vec3 color) {
	_specularColor = color;
}

void TerrainMaterial::setShininess(float value) {
	_shininess = value;
}

void TerrainMaterial::setDiffuseTexture1 (Texture* pDiffuseTexture) {
	_diffuseTexture1 = pDiffuseTexture;
}

void TerrainMaterial::setDiffuseTexture2(Texture* pDiffuseTexture) {
	_diffuseTexture2 = pDiffuseTexture;
}

void TerrainMaterial::setDiffuseTexture3(Texture* pDiffuseTexture) {
	_diffuseTexture3 = pDiffuseTexture;
}

void TerrainMaterial::setDiffuseTexture4(Texture* pDiffuseTexture) {
	_diffuseTexture4 = pDiffuseTexture;
}

void TerrainMaterial::setHeightMapTexture(Texture* pHeightMapTexture) {
	_heightMapTexture = pHeightMapTexture;
}

void TerrainMaterial::setSplatMapTexture(Texture* pSplatMapTexture) {
	_splatMapTexture = pSplatMapTexture;
}

void TerrainMaterial::render(World* pWorld, Mesh* pMesh, const glm::mat4& pModelMatrix, const glm::mat4& pViewMatrix, const glm::mat4& pProjectionMatrix) {
    if (!_diffuseTexture1 || !_diffuseTexture2 || !_diffuseTexture3 || !_diffuseTexture4 || !_heightMapTexture || !_splatMapTexture) return;

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

		glUniform3fv(_shader->getUniformLocation(lightString + "lightForward"), 1, glm::value_ptr(currentLight->getTransform()[2])); //not sure why this needs to be transposed for a terrain
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
	glUniform3fv(_shader->getUniformLocation("specularColor"), 1, glm::value_ptr(_specularColor));
	glUniform1f(_shader->getUniformLocation("shininess"), _shininess);

	//passing in camera position
	glUniform3fv(_shader->getUniformLocation("eyePosition"), 1, glm::value_ptr(mainCam->getWorldPosition())); //might need to be changed to world pos

	//loading heightmap into slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _heightMapTexture->getId());
	glUniform1i(_shader->getUniformLocation("heightMapTexture"), 0);

	//loading splatmap into slot 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _splatMapTexture->getId());
	glUniform1i(_shader->getUniformLocation("splatMapTexture"), 1);

	//loading diffuse1 into slot 2
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _diffuseTexture1->getId());
	glUniform1i(_shader->getUniformLocation("diffuseTexture1"), 2);

	//loading diffuse2 into slot 3
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, _diffuseTexture2->getId());
	glUniform1i(_shader->getUniformLocation("diffuseTexture2"), 3);

	//loading diffuse3 into slot 4
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, _diffuseTexture3->getId());
	glUniform1i(_shader->getUniformLocation("diffuseTexture3"), 4);

	//loading diffuse4 into slot 5
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, _diffuseTexture4->getId());
	glUniform1i(_shader->getUniformLocation("diffuseTexture4"), 5);

	glUniform1f(_shader->getUniformLocation("time"), glm::float1(std::clock())); //passing in the time to the vertex shader so that we can animate stuff

	//pass in all MVP matrices separately
	glUniformMatrix4fv(_shader->getUniformLocation("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(pProjectionMatrix));
	glUniformMatrix4fv(_shader->getUniformLocation("viewMatrix"), 1, GL_FALSE, glm::value_ptr(pViewMatrix));
	glUniformMatrix4fv(_shader->getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(pModelMatrix));

    //now inform mesh of where to stream its data
    pMesh->streamToOpenGL(_aVertex, _aNormal, _aUV);
}
