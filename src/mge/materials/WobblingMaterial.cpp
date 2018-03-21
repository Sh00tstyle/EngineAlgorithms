#include "glm.hpp"

#include "mge/materials/WobblingMaterial.h"
#include "mge/config.hpp"
#include "mge/core/GameObject.hpp"
#include "mge/core/Mesh.hpp"
#include "mge/core/ShaderProgram.hpp"


ShaderProgram* WobblingMaterial::_shader = NULL;

WobblingMaterial::WobblingMaterial() : AbstractMaterial() {
	//constructor

	_lazyInitializeShader();
}

WobblingMaterial::~WobblingMaterial() {
	//destructor
}

void WobblingMaterial::_lazyInitializeShader() {
	//only using one shaderprogram of every single wobbling material instance
	if(_shader == NULL) {
		_shader = new ShaderProgram();
		_shader->addShader(GL_VERTEX_SHADER, config::MGE_SHADER_PATH + "wobbling.vs");
		_shader->addShader(GL_FRAGMENT_SHADER, config::MGE_SHADER_PATH + "wobbling.fs");
		_shader->finalize();
	}
}

void WobblingMaterial::render(World* pWorld, Mesh* pMesh, const glm::mat4& pModelMatrix, const glm::mat4& pViewMatrix, const glm::mat4& pProjectionMatrix) {
	_shader->use();

	//passing in the time to the vertex shader so that we can animate stuff
	glUniform1f(_shader->getUniformLocation("time"), glm::float1(std::clock())); 

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