#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "GL/GLDefine.h"
#include "GL/GPUResource/Shader/GLProgram.h"
#include "Common/Tools/Utils.h"
#include "Common/Components/Reference.h"
#include "GL/GPUResource/Model/Mesh.h"
#include "Browser/Components/Camera/Camera.h"


using namespace std;

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
		// 使用材质
		void usePass(bool transformDirty, const glm::mat4& modelMatrix, bool cameraDirty, const glm::vec3& cameraGlobalPos, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, std::unordered_map<std::string, UniformValue>& uniforms);
		// GLProgram是否在GPU中加载完成
		bool isGPUResourceLoaded();

		REGISTER_PROPERTY_GET(GLProgram*, m_oGLProgram, GLProgram)

		// depth test
		REGISTER_PROPERTY_SET(bool, m_bEnableZTest, EnableZTest)
		REGISTER_PROPERTY_SET(GLenum, m_eZTestFunc, ZTestFunc)
		REGISTER_PROPERTY_SET(bool, m_bZWrite, ZWrite)
		// stencil test
		void setEnableStencilTest(bool enable);
		void setStencilFuncParameter(GLint ref, GLuint mask);
		void setStencilFuncParameter(GLenum func, GLint ref, GLuint mask);
		void setStencilOpParameter(GLenum sfail, GLenum dpfail, GLenum dppass);
		// blend
		void setEnableBlend(bool enable);
		void setBlendFuncParameter(GLenum sfactor, GLenum dfactor);
		void setBlendFuncParameter(GLenum sfactor, GLenum dfactor, GLenum safactor, GLenum dafactor);
		REGISTER_PROPERTY_SET(GLenum, m_eBlendEquation, BlendEquation)

	private:
		// 对应的shader程序
		GLProgram* m_oGLProgram;
		// UniformValues是否被初始化
		bool m_bInitUniforms;

		// depth
		// 深度测试开关
		bool m_bEnableZTest;
		// 深度测试方法
		GLenum m_eZTestFunc;
		// 深度写入
		bool m_bZWrite;

		// stencil
		// 模板测试开关
		bool m_bEnableStencilTest;
		// 模板测试方法
		std::shared_ptr<StencilFuncParameter> m_pStencilFuncParam;
		// 模板缓冲更新
		std::shared_ptr<StencilOpParameter> m_pStencilOpParam;

		// Blend
		// 混合开关
		bool m_bEnableBlend;
		// 混合因子		
		std::shared_ptr<BlendFuncParameter> m_pBlendFuncParam;
		// 混合操作(运算符)
		GLenum m_eBlendEquation;
	};
}

