#ifndef LINERENDERER_HPP
#define LINERENDERER_HPP

#include <vector>
#include <GL/glew.h>
#include "glm.hpp"

class World;
class BoundingBox;
class ShaderProgram;

class LineRenderer
{
	public:
		LineRenderer(BoundingBox* pBounds, bool pIsOctree = false);
		~LineRenderer();

		void render(const glm::mat4& pModelMatrix, const glm::mat4& pViewMatrix, const glm::mat4& pProjectionMatrix);

		void setLineColor(glm::vec4 color);

	private:
		static ShaderProgram* _shader;

		GLfloat _vertices[72]; //vertex position data

		GLuint _vbo; //vertex buffer object
		GLuint _vao; //vertex array object

		glm::vec4 _lineColor;
       
		void _buffer(); //buffer vertices
		void _initShader(); //lazy init the shader (create if it doesnt exist)

		void _generateVertices(BoundingBox* bounds, bool isOctree = false); //generate vertices from a cube
};

#endif // LINERENDERER_HPP