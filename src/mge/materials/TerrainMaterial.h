#include "mge/materials/AbstractMaterial.hpp"
#include "GL/glew.h"

class ShaderProgram;
class Texture;

/**
 * This material is already a little bit more complicated, instead of a color we can pass in a texture,
 * all attributes and uniforms are cached and we precompute the MVP matrix passing it in as one entity.
 */
class TerrainMaterial : public AbstractMaterial
{
    public:
		TerrainMaterial(glm::vec3 pAmbientColor, glm::vec3 pSpecularColor, float pShininess, Texture* pDiffuseTexture1, Texture* pDiffuseTexture2, Texture* pDiffuseTexture3, Texture* pDiffuseTexture4, 
						Texture* pHeightMapTexture, Texture* pSplatMapTexture);
        virtual ~TerrainMaterial();

        virtual void render(World* pWorld, Mesh* pMesh, const glm::mat4& pModelMatrix, const glm::mat4& pViewMatrix, const glm::mat4& pProjectionMatrix) override;

		void setAmbientColor(glm::vec3 color);
		void setSpecularColor(glm::vec3 color);
		void setShininess(float value);

        void setDiffuseTexture1 (Texture* pDiffuseTexture);
		void setDiffuseTexture2(Texture* pDiffuseTexture);
		void setDiffuseTexture3(Texture* pDiffuseTexture);
		void setDiffuseTexture4(Texture* pDiffuseTexture);
		void setHeightMapTexture(Texture* pHeightMapTexture);
		void setSplatMapTexture(Texture* pSplatMapTexture);

    protected:
    private:
        static ShaderProgram* _shader;
        static void _lazyInitializeShader();

        static GLint _aVertex ;
        static GLint _aNormal;
        static GLint _aUV ;

		glm::vec3 _ambientColor;
		glm::vec2 _specularColor;
		float _shininess;

        Texture* _diffuseTexture1;
		Texture* _diffuseTexture2;
		Texture* _diffuseTexture3;
		Texture* _diffuseTexture4;
		Texture* _heightMapTexture;
		Texture* _splatMapTexture;

		TerrainMaterial(const TerrainMaterial&);
		TerrainMaterial& operator=(const TerrainMaterial&);

};