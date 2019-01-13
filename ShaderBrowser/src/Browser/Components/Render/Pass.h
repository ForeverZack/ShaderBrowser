#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include "../../../GL/GLDefine.h"
#include "../../../GL/GLProgram.h"
#include "../../../Common/Tools/Utils.h"


using namespace std;
using namespace customGL;

namespace browser
{
	class Pass
	{
	public:
		// 
		static Pass* createPass(GLProgram* program);

	public:
		Pass();
		~Pass();

	public:
		// 初始化材质
		void init(GLProgram* program);
		// 添加\设置uniform
        void setUniformInt(const char* uniformName, int value);
        void setUniformFloat(const char* uniformName, float value);
        void setUniformMat3(const char* uniformName, glm::mat3 value);
        void setUniformMat4(const char* uniformName, glm::mat4 value);
        void setUniformFloatV(const char* uniformName, int size, const float* value);
        void setUniformV2f(const char* uniformName, glm::vec2 value);
        void setUniformV3f(const char* uniformName, glm::vec3 value);
		void setUniformV4f(const char* uniformName, glm::vec4 value);
        void setUniformTex2D(const char* uniformName, GLuint textureId);
		// 使用材质
		void usePass();


		REGISTER_PROPERTY_GET(GLProgram*, m_oGLProgram, GLProgram)

	private:
		// uniform数据
		std::unordered_map<const char*, UniformValue> m_mUniforms;
		// 对应的shader程序
		GLProgram* m_oGLProgram;
	};
}

