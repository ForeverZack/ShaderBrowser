#include "Pass.h"
#include "GL/GPUResource/Texture/Texture2D.h"
#include "GL/GLDefine.h"

namespace customGL
{
	Pass* Pass::createPass(GLProgram* program)
	{
		Pass* pass = new Pass();
		pass->init(program);
        

		return pass;
	}

	Pass::Pass()
		: m_oGLProgram(nullptr)
		, m_bInitUniforms(false)
		// depth test
		, m_bEnableZTest(true)
		, m_eZTestFunc(GL_LESS)
		, m_bZWrite(true)
		// stencil test
		, m_bEnableStencilTest(false)
		, m_pStencilFuncParam(nullptr)
		, m_pStencilOpParam(nullptr)
		// blend
		, m_bEnableBlend(false)
		, m_pBlendFuncParam(nullptr)
		, m_eBlendEquation(GL_FUNC_ADD)
	{
	}

	Pass::~Pass()
	{
        if (m_oGLProgram)
        {
            m_oGLProgram->release();
        }
	}

	void Pass::init(GLProgram* program)
	{
        program->retain();
		m_oGLProgram = program;
	}

	bool Pass::isGPUResourceLoaded()
	{
		return m_oGLProgram->getResouceState() == GRS_Loaded;
	}

	void Pass::usePass(bool transformDirty, const glm::mat4& modelMatrix, bool cameraDirty, const glm::vec3& cameraGlobalPos, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, std::unordered_map<std::string, UniformValue>& uniforms)
	{
        // 注意在更新uniform变量的值之前，要先使用着色器glUseProgram
		m_oGLProgram->useProgram();

		// 更新属性
		bool forceUpdate = !m_bInitUniforms;	// 第一次使用pass的时候，要强制更新所有Uniform
		for (auto itor = uniforms.begin(); itor != uniforms.end(); ++itor)
		{
			itor->second.updateGLProgramUniformValue(m_oGLProgram, itor->first, forceUpdate);
		}

		// 更新mvp
		if (transformDirty)
		{
			m_oGLProgram->setUniformWithMat4(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_MODEL_MATRIX], modelMatrix);
		}
		if (cameraDirty)
		{
            m_oGLProgram->setUniformWithVec3(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_MAINCAMERA_WORLDPOS], cameraGlobalPos.x, cameraGlobalPos.y, cameraGlobalPos.z);
			m_oGLProgram->setUniformWithMat4(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_VIEW_MATRIX], viewMatrix);
			m_oGLProgram->setUniformWithMat4(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_PROJECTION_MATRIX], projectionMatrix);
		}

		// 初始化完成
		m_bInitUniforms = true;
	}

	void Pass::setEnableStencilTest(bool enable)
	{
		m_bEnableStencilTest = enable;
		if (enable)
		{
			if (!m_pStencilFuncParam)
			{
				m_pStencilFuncParam = make_shared<StencilFuncParameter>(0, 0xFF);
			}
			if (!m_pStencilOpParam)
			{
				m_pStencilOpParam = make_shared<StencilOpParameter>(GL_KEEP, GL_KEEP, GL_REPLACE);
			}
		}
	}

	void Pass::setStencilFuncParameter(GLint ref, GLuint mask)
	{
		if (!m_pStencilFuncParam)
		{
			m_pStencilFuncParam = make_shared<StencilFuncParameter>(ref, mask);
		}
		else
		{
			m_pStencilFuncParam->func = GL_EQUAL;
			m_pStencilFuncParam->ref = ref;
			m_pStencilFuncParam->mask = mask;
		}
	}

	void Pass::setStencilFuncParameter(GLenum func, GLint ref, GLuint mask)
	{
		if (!m_pStencilFuncParam)
		{
			m_pStencilFuncParam = make_shared<StencilFuncParameter>(func, ref, mask);
		}
		else
		{
			m_pStencilFuncParam->func = func;
			m_pStencilFuncParam->ref = ref;
			m_pStencilFuncParam->mask = mask;
		}
	}

	void Pass::setStencilOpParameter(GLenum sfail, GLenum dpfail, GLenum dppass)
	{
		if (!m_pStencilOpParam)
		{
			m_pStencilOpParam = make_shared<StencilOpParameter>(sfail, dpfail, dppass);
		}
		else
		{
			m_pStencilOpParam->sfail = sfail;
			m_pStencilOpParam->dpfail = dpfail;
			m_pStencilOpParam->dppass = dppass;
		}
	}

	void Pass::setEnableBlend(bool enable)
	{
		m_bEnableBlend = enable;
		if (enable)
		{
			if (!m_pBlendFuncParam)
			{
				m_pBlendFuncParam = std::make_shared<BlendFuncParameter>(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
		}
	}

	void Pass::setBlendFuncParameter(GLenum sfactor, GLenum dfactor)
	{
		if (!m_pBlendFuncParam)
		{
			m_pBlendFuncParam = std::make_shared<BlendFuncParameter>(sfactor, dfactor);
		}
		else
		{
			m_pBlendFuncParam->sfactor = sfactor;
			m_pBlendFuncParam->dfactor = dfactor;
			m_pBlendFuncParam->safactor = sfactor;
			m_pBlendFuncParam->dafactor = dfactor;
		}
	}

	void Pass::setBlendFuncParameter(GLenum sfactor, GLenum dfactor, GLenum safactor, GLenum dafactor)
	{
		if (!m_pBlendFuncParam)
		{
			m_pBlendFuncParam = std::make_shared<BlendFuncParameter>(sfactor, dfactor, safactor, dafactor);
		}
		else
		{
			m_pBlendFuncParam->sfactor = sfactor;
			m_pBlendFuncParam->dfactor = dfactor;
			m_pBlendFuncParam->safactor = safactor;
			m_pBlendFuncParam->dafactor = dafactor;
		}
	}


}
