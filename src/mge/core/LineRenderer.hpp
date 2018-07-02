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
		LineRenderer(BoundingBox* pBounds);
		~LineRenderer();

		void render(const glm::mat4& pModelMatrix, const glm::mat4& pViewMatrix, const glm::mat4& pProjectionMatrix);

	private:
		static ShaderProgram* _shader;

		GLuint _indexBufferId;
		GLuint _vertexBufferId;

	    //the actual data
		std::vector<unsigned> _indices;
		std::vector<glm::vec3> _vertices;       //vec3 with 3d coords for all vertices

        //buffer vertices, normals, and uv's
		void _buffer();

		void _initShader();

		//generate vertices from a cube
		void _generateVertices(BoundingBox* bounds);

		void _streamToOpenGL();
};

#endif // LINEMESH_HPP