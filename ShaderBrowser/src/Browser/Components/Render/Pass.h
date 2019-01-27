#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include "../../../GL/GLDefine.h"
#include "../../../GL/GLProgram.h"
#include "../../../Common/Tools/Utils.h"
#include "../Mesh/Mesh.h"


using namespace std;
using namespace customGL;

/*
	注意：pass是可复用的对象，他的作用类似于cocos的glProgramState，用来保存uniform变量的值（两个pass可能会使用同一个glProgram，但其中的uniform可能不相同）
*/

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
        void setUniformInt(const std::string& uniformName, int value);
        void setUniformFloat(const std::string& uniformName, float value);
        void setUniformMat3(const std::string& uniformName, glm::mat3 value);
        void setUniformMat4(const std::string& uniformName, glm::mat4 value);
        void setUniformFloatV(const std::string& uniformName, int size, const float* value);
        void setUniformV2f(const std::string& uniformName, glm::vec2 value);
        void setUniformV3f(const std::string& uniformName, glm::vec3 value);
		void setUniformV4f(const std::string& uniformName, glm::vec4 value);
        void setUniformTex2D(const std::string& uniformName, GLuint textureId);
		// 使用材质
		void usePass(Mesh* mesh);


		REGISTER_PROPERTY_GET(GLProgram*, m_oGLProgram, GLProgram)

	private:
		// uniform数据
        std::unordered_map<std::string, UniformValue> m_mUniforms;
		// 对应的shader程序
		GLProgram* m_oGLProgram;
	};
}

