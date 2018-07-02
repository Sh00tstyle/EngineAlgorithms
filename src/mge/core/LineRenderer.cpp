#include <iostream>
#include <map>
#include <string>
#include <fstream>

#include "mge/core/LineRenderer.hpp"
#include "mge/octree/BoundingBox.h"
#include "mge/core/ShaderProgram.hpp"
#include "mge/config.hpp"

ShaderProgram* LineRenderer::_shader = nullptr;

LineRenderer::LineRenderer(BoundingBox* pBounds) {
	_generateVertices(pBounds);

	_initShader();
	_buffer();
}

LineRenderer::~LineRenderer() {
	//dtor
}

void LineRenderer::render(const glm::mat4& pModelMatrix, const glm::mat4& pViewMatrix, const glm::mat4& pProjectionMatrix) {
	/*_shader->use();

	//set the material color
	glUniform3fv(_shader->getUniformLocation("diffuseColor"), 1, glm::value_ptr(glm::vec3(1, 1, 1))); //TODO: set color for lines (?)

	//pass in all MVP matrices separately
	glUniformMatrix4fv(_shader->getUniformLocation("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(pProjectionMatrix));
	glUniformMatrix4fv(_shader->getUniformLocation("viewMatrix"), 1, GL_FALSE, glm::value_ptr(pViewMatrix));
	glUniformMatrix4fv(_shader->getUniformLocation("modelMatrix"), 1, GL_FALSE, glm::value_ptr(pModelMatrix));*/

	//now inform mesh of where to stream its data
	_streamToOpenGL();
}



void LineRenderer::_buffer() {
	glGenBuffers(1, &_indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), &_indices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &_vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(glm::vec3), &_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LineRenderer::_initShader() {
	if(_shader == nullptr) {
		_shader = new ShaderProgram();
		_shader->addShader(GL_VERTEX_SHADER, config::MGE_SHADER_PATH + "line.vs");
		_shader->addShader(GL_FRAGMENT_SHADER, config::MGE_SHADER_PATH + "line.fs");
		_shader->finalize();
	}
}

void LineRenderer::_generateVertices(BoundingBox * bounds) {
	//store relevant bound data
	glm::vec3 center = bounds->getCenter();
	glm::vec3 halfSize = bounds->getHalfSize();

	//calculate edge points
	glm::vec3 v1 = center + glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z); //back, bottom, left
	glm::vec3 v2 = center + glm::vec3(halfSize.x, -halfSize.y, -halfSize.z); //back, bottom, right
	glm::vec3 v3 = center + glm::vec3(-halfSize.x, -halfSize.y, halfSize.z); //front, bottom, left
	glm::vec3 v4 = center + glm::vec3(halfSize.x, -halfSize.y, halfSize.z); //front, bottom, right
	glm::vec3 v5 = center + glm::vec3(-halfSize.x, halfSize.y, -halfSize.z); //back, top, left
	glm::vec3 v6 = center + glm::vec3(halfSize.x, halfSize.y, -halfSize.z); //back, top, right
	glm::vec3 v7 = center + glm::vec3(-halfSize.x, halfSize.y, halfSize.z); //front, top, left
	glm::vec3 v8 = center + glm::vec3(halfSize.x, halfSize.y, halfSize.z); //front, top, right

	_vertices.clear();

	//store in list
	glm::vec3 vertices[24] = {
		v1, v2,
		v1, v3,
		v2, v4,
		v3, v4,
		v5, v6,
		v5, v7,
		v6, v8,
		v7, v8,
		v1, v5,
		v2, v6,
		v3, v7,
		v4, v8
	};

	for(int i = 0; i < sizeof(vertices); i++) {
		_vertices.push_back(vertices[i]);
	}

	//init indices (?)
	for(int i = 0; i < _vertices.size(); i++) {
		_indices.push_back(i);
	}
}

void LineRenderer::_streamToOpenGL() {
	glBegin(GL_LINES);

	/**
	for(unsigned i = 0; i < _vertices.size(); i++) {
		//draw line
		glm::vec3 vertex = _vertices[i];
		glVertex3fv(glm::value_ptr(vertex));

	}
	/**/

	glEnd();
}