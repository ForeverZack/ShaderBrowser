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
	// depth
	class PassZTestParameter
	{
	public:
		PassZTestParameter()
			: enableZTest(true)
			, ZTestFunc(GL_LESS)
			, ZWrite(true)
		{
		}
		~PassZTestParameter()
		{
		}

	public:
		// 深度测试开关
		bool enableZTest;
		// 深度测试方法
		GLenum ZTestFunc;
		// 深度写入
		bool ZWrite;
	};

	// cull
	class PassCullParameter
	{
	public:
		PassCullParameter()
			: enableCull(true)
			, cullFace(GL_FRONT)
			, frontFace(GL_CCW)
		{
		}
		~PassCullParameter()
		{
		}

	public:
		// 面剔除开关
		bool enableCull;
		// 剔除面的类型
		GLenum cullFace;
		// 正面是顺时针还是逆时针 (默认值是逆时针)	
		GLenum frontFace;
	};

	// stencil
	class PassStencilParameter
	{
	public:
		PassStencilParameter()
			: enableStencilTest(false)
		{
			stencilFuncParam = make_shared<StencilFuncParameter>(0, 0xFF);
			stencilOpParam = make_shared<StencilOpParameter>(GL_KEEP, GL_KEEP, GL_REPLACE);
		}
		~PassStencilParameter()
		{
		}

	public:
		// 模板测试开关
		bool enableStencilTest;
		// 模板测试方法
		std::shared_ptr<StencilFuncParameter> stencilFuncParam;
		// 模板缓冲更新
		std::shared_ptr<StencilOpParameter> stencilOpParam;
	};

	// blend
	class PassBlendParameter
	{
	public:
		PassBlendParameter()
			: enableBlend(false)
			, blendEquation(GL_FUNC_ADD)
		{
			blendFuncParam = std::make_shared<BlendFuncParameter>(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		~PassBlendParameter()
		{
		}

	public:
		// 混合开关
		bool enableBlend;
		// 混合因子		
		std::shared_ptr<BlendFuncParameter> blendFuncParam;
		// 混合操作(运算符)
		GLenum blendEquation;
	};

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
		void usePass(bool transformDirty, const glm::mat4& modelMatrix, bool cameraDirty, const glm::vec3& cameraGlobalPos, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, std::unordered_map<std::string, UniformValue>& uniforms
							, bool enableZTest, GLenum ZTestFunc, bool ZWrite
							, bool enableCull, GLenum cullFace, GLenum frontFace
							, bool enableStencilTest, std::shared_ptr<StencilFuncParameter> stencilFuncParam, std::shared_ptr<StencilOpParameter> stencilOpParam
							, bool enableBlend, std::shared_ptr<BlendFuncParameter> blendFuncParam, GLenum blendEquation);
		// GLProgram是否在GPU中加载完成
		bool isGPUResourceLoaded();

		REGISTER_PROPERTY_GET(GLProgram*, m_oGLProgram, GLProgram)

		// depth test
		void setEnableZTest(bool enableZTest);
		void setZTestFunc(GLenum ZTestFunc);
		void setZWrite(bool ZWrite);
		// cull face
		void setEnableCull(bool enable);
		void setCullFace(GLenum cullFace);
		void setFrontFace(GLenum frontFace);
		// stencil test
		void setEnableStencilTest(bool enable);
		void setStencilFuncParameter(GLint ref, GLuint mask);
		void setStencilFuncParameter(GLenum func, GLint ref, GLuint mask);
		void setStencilOpParameter(GLenum sfail, GLenum dpfail, GLenum dppass);
		// blend
		void setEnableBlend(bool enable);
		void setBlendFuncParameter(GLenum sfactor, GLenum dfactor);
		void setBlendFuncParameter(GLenum sfactor, GLenum dfactor, GLenum safactor, GLenum dafactor);
		void setBlendEquation(GLenum blendEquation);

	private:
		// 对应的shader程序
		GLProgram* m_oGLProgram;
		// UniformValues是否被初始化
		bool m_bInitUniforms;

		// depth
		std::shared_ptr<PassZTestParameter> m_pZTestParameter;

		// cull
		std::shared_ptr<PassCullParameter> m_pCullParameter;

		// stencil
		std::shared_ptr<PassStencilParameter> m_pStencilParameter;

		// Blend
		std::shared_ptr<PassBlendParameter> m_pBlendParameter;
	};
}

