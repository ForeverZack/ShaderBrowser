#ifndef COMS_RENDER_MATERIAL_H
#define COMS_RENDER_MATERIAL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Pass.h"
#include "GL/GLProgram.h"
#include "Common/Tools/Utils.h"
#include "Browser/Components/Mesh/Mesh.h"
#include "Browser/Components/Camera/Camera.h"

using namespace std;
using namespace customGL;

namespace browser
{
    
    class Material : public Reference
	{
	public:
		// 默认材质名称
		static const char* DEFAULT_MATERIAL_NAME;

	public:
		// TODO: 这里的vao应该是生成material之后自动生成的(可以从缓存中找)
		static Material* createMaterial(const std::string& materialName = DEFAULT_MATERIAL_NAME);

	public:
		Material(const std::string& materialName);
		~Material();

	public:
		// 初始化材质
		void init();
        // 添加pass
        void addPass(Pass* pass);
		// 使用材质的第几个Pass
		// TODO: 这里实际上应该根据pass的类型来决定，而不是根据index ( 这里的类型也跟渲染队列有关，比方说如果在进行阴影深度贴图的渲染，应该使用对应的简单的pass )
		void useMaterial(Mesh* mesh, Transform* transform, Camera* camera, int index = 0);
		Pass* getUsePass(int index = 0);


		REGISTER_PROPERTY_CONSTREF_GET(std::vector<Pass*>, m_vPass, Pass)
		REGISTER_PROPERTY_CONSTREF_GET(std::string, m_sMaterialName, MaterialName)

	private:
		// 名称
		std::string m_sMaterialName;
        // uniform数据
        std::unordered_map<std::string, UniformValue> m_mUniforms;
        
		// Pass队列
		std::vector<Pass*> m_vPass;
	};
}

#endif
