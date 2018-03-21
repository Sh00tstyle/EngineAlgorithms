#include "GL/glew.h"
#include "mge/materials/AbstractMaterial.hpp"

class ShaderProgram;

class LitMaterial : public AbstractMaterial
{
    public:
	    LitMaterial(glm::vec3 pAmbientColor, glm::vec3 pDiffuseColor, glm::vec3 pSpecularColor, float pShininess);
        virtual ~LitMaterial();

        virtual void render(World* pWorld, Mesh* pMesh, const glm::mat4& pModelMatrix, const glm::mat4& pViewMatrix, const glm::mat4& pProjectionMatrix) override;

        //in rgb values
		void setAmbientColor(glm::vec3 pAmbientColor);
        void setDiffuseColor (glm::vec3 pDiffuseColor);
		void setSpecularColor(glm::vec3 pSpecularColor);
		void setShininess(float pShininess);

    private:
        //all the static properties are shared between instances of ColorMaterial
        //note that they are all PRIVATE, we do not expose this static info to the outside world
        static ShaderProgram* _shader;
        static void _lazyInitializeShader();

		glm::vec3 _ambientColor;
        glm::vec3 _diffuseColor;
		glm::vec3 _specularColor;
		float _shininess;
};