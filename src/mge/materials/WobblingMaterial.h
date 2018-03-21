#include "mge/materials/AbstractMaterial.hpp"

class ShaderProgram; //needed, to use the ShaderProgram class in the header

class WobblingMaterial : public AbstractMaterial {
	public:
	WobblingMaterial();
	virtual ~WobblingMaterial();

	virtual void render(World* pWorld, Mesh* pMesh, const glm::mat4& pModelMatrix, const glm::mat4& pViewMatrix, const glm::mat4& pProjectionMatrix) override;

	private:
	//shared between every single instance of the wobbling material
	static ShaderProgram* _shader;
	static void _lazyInitializeShader();
};