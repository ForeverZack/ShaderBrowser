#ifndef COMS_RENDER_MATERIAL_H
#define COMS_RENDER_MATERIAL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Pass.h"
#include "../../../GL/GLProgram.h"
#include "../../../Common/Tools/Utils.h"

using namespace std;
using namespace customGL;

namespace browser
{
	class Material
	{
	public:
		// TODO: 这里的vao应该是生成material之后自动生成的(可以从缓存中找)
		static Material* createMaterial();

	public:
		Material();
		~Material();

	public:
		// 初始化材质
		void init();
        // 添加pass
        void addPass(Pass* pass);
		// 使用材质的第几个Pass
		void useMaterial(int index = 0);


        // 返回Pass队列
        REGISTER_PROPERTY_GET(std::vector<Pass*>, m_vPass, Pass)

	private:
		// Pass队列
		std::vector<Pass*> m_vPass;
	};
}

#endif
