#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include "GL/GLDefine.h"
#include "GL/GPUResource/Shader/GLProgram.h"
#include "Common/Tools/Utils.h"
#include "Common/Components/Reference.h"
#include "GL/GPUResource/Model/Mesh.h"
#include "Browser/Components/Camera/Camera.h"



using namespace std;
using namespace customGL;

/*
	注意：pass是可复用的对象，他的作用类似于cocos的glProgramState，用来保存uniform变量的值（两个pass可能会使用同一个glProgram，但其中的uniform可能不相同）
*/

namespace customGL
{
    class Pass : public common::Reference
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
        // 设置参数列表（从所属的material获得）
        void setUniformsFromMaterial(std::unordered_map<std::string, UniformValue>* uniforms);
		// 使用材质
		void usePass();


		REGISTER_PROPERTY_GET(GLProgram*, m_oGLProgram, GLProgram)

	private:
		// uniform数据指针（从所属material获得）
        std::unordered_map<std::string, UniformValue>* m_mUniforms;
		// 对应的shader程序
		GLProgram* m_oGLProgram;
	};
}

