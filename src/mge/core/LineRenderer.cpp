#include <iostream>
#include <map>
#include <string>
#include <fstream>

#include "mge/core/LineRenderer.hpp"
#include "mge/octree/BoundingBox.h"
#include "mge/core/ShaderProgram.hpp"
#include "mge/config.hpp"

ShaderProgram* LineRenderer::_shader = nullptr;

LineRenderer::LineRenderer(BoundingBox* pBounds, bool pIsOctree) {
	//init the line renderer
	_generateVertices(pBounds, pIsOctree);
	_lineColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); //green

	_initShader();
	_buffer();
}

LineRenderer::~LineRenderer() {
	//cleanup gpu memory
	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_vbo);
}

void LineRenderer::render(const glm::mat4& pModelMatrix, const glm::mat4& pViewMatrix, const glm::mat4& pProjectionMatrix) {
	//use the line shader
	_shader->use();

	//pass in all matrices for the MVP matrix seperately
	glUniformMatrix4fv(_shader->getUniformLocation("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(pProjectionMatrix));
	glUniformMatrix4fv(_shader->getUniformLocation("viewMatrix"), 1, GL_FALSE, glm::value_ptr(pViewMatrix));
	glUniformMatrix4fv(_shader->getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(pModelMatrix));

	//pass in line color
	glUniform4fv(_shader->getUniformLocation("lineColor"), 1, glm::value_ptr(_lineColor));

	//use vertex array
	glBindVertexArray(_vao);

	//draw lines from the vao based on the amount of positions in the vertex array
	glDrawArrays(GL_LINES, 0, sizeof(_vertices) / sizeof(*_vertices) / 3);

	//unbind
	glBindVertexArray(0);
}

void LineRenderer::setLineColor(glm::vec4 color) {
	_lineColor = color;
}

void LineRenderer::_buffer() {
	//generate vertex buffer object to store the vertex information
	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	//allocate video memory and store the data in the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), _vertices, GL_STATIC_DRAW);

	//generate and bind vertex array object
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	//get the index for the vertex attribute and store the information in the currently bound vao
	GLint vertexAttrib = _shader->getAttribLocation("vertex");
	glEnableVertexAttribArray(vertexAttrib);
	glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//unbind the vao and vbo to avoid errors
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void LineRenderer::_initShader() {
	//lazy init shader, so create it if it doesnt exist already
	if(_shader == nullptr) {
		_shader = new ShaderProgram();
		_shader->addShader(GL_VERTEX_SHADER, config::MGE_SHADER_PATH + "line.vs");
		_shader->addShader(GL_FRAGMENT_SHADER, config::MGE_SHADER_PATH + "line.fs");
		_shader->finalize();
	}
}

void LineRenderer::_generateVertices(BoundingBox * bounds, bool isOctree) {
	// store relevant boundary data
	glm::vec3 center = glm::vec3(0, 0, 0);
	if(isOctree) center = bounds->getCenter(); //not dependant on the gameobject pos
	glm::vec3 halfSize = bounds->getHalfSize();

	//calculate edge points in local space
	glm::vec3 v1 = center + glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z); //back, bottom, left
	glm::vec3 v2 = center + glm::vec3(halfSize.x, -halfSize.y, -halfSize.z); //back, bottom, right
	glm::vec3 v3 = center + glm::vec3(-halfSize.x, -halfSize.y, halfSize.z); //front, bottom, left
	glm::vec3 v4 = center + glm::vec3(halfSize.x, -halfSize.y, halfSize.z); //front, bottom, right
	glm::vec3 v5 = center + glm::vec3(-halfSize.x, halfSize.y, -halfSize.z); //back, top, left
	glm::vec3 v6 = center + glm::vec3(halfSize.x, halfSize.y, -halfSize.z); //back, top, right
	glm::vec3 v7 = center + glm::vec3(-halfSize.x, halfSize.y, halfSize.z); //front, top, left
	glm::vec3 v8 = center + glm::vec3(halfSize.x, halfSize.y, halfSize.z); //front, top, right

    //store in array with render order
	GLfloat vertices[] = {
		v1.x, v1.y, v1.z, v2.x, v2.y, v2.z,
		v1.x, v1.y, v1.z, v3.x, v3.y, v3.z,
		v2.x, v2.y, v2.z, v4.x, v4.y, v4.z,
		v3.x, v3.y, v3.z, v4.x, v4.y, v4.z,
		v5.x, v5.y, v5.z, v6.x, v6.y, v6.z,
		v5.x, v5.y, v5.z, v7.x, v7.y, v7.z,
		v6.x, v6.y, v6.z, v8.x, v8.y, v8.z,
		v7.x, v7.y, v7.z, v8.x, v8.y, v8.z,
		v1.x, v1.y, v1.z, v5.x, v5.y, v5.z,
		v2.x, v2.y, v2.z, v6.x, v6.y, v6.z,
		v3.x, v3.y, v3.z, v7.x, v7.y, v7.z,
		v4.x, v4.y, v4.z, v8.x, v8.y, v8.z
	};

	//copy local array to member array
	std::copy(std::begin(vertices), std::end(vertices), std::begin(_vertices));
}