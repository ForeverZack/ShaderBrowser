#include "Pass.h"
#include "GL/GPUResource/Texture/Texture2D.h"
#include "GL/GLDefine.h"
#include "GL/GLStateCache.h"

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
		, m_pZTestParameter(nullptr)
		// stencil test
		, m_pStencilParameter(nullptr)
		// blend
		, m_pBlendParameter(nullptr)
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

	void Pass::usePass(bool transformDirty, const glm::mat4& modelMatrix, bool cameraDirty, const glm::vec3& cameraGlobalPos, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, std::unordered_map<std::string, UniformValue>& uniforms
		, bool enableZTest, GLenum ZTestFunc, bool ZWrite, bool enableStencilTest, std::shared_ptr<StencilFuncParameter> stencilFuncParam, std::shared_ptr<StencilOpParameter> stencilOpParam
		, bool enableBlend, std::shared_ptr<BlendFuncParameter> blendFuncParam, GLenum blendEquation)
	{
		// base
		usePass(transformDirty, modelMatrix, cameraDirty, cameraGlobalPos, viewMatrix, projectionMatrix, uniforms);

		// 设置GL状态
		// z test
		if (m_pZTestParameter)
		{
			// 如果pass设置了z test相关参数
			GLStateCache::getInstance()->setZTest(m_pZTestParameter->enableZTest, m_pZTestParameter->ZTestFunc, m_pZTestParameter->ZWrite);
		}
		else
		{
			// 否则，默认使用material的参数
			GLStateCache::getInstance()->setZTest(enableZTest, ZTestFunc, ZWrite);
		}
		// stencil test
		if (m_pStencilParameter)
		{
			// 如果pass设置了stencil test相关参数
			GLStateCache::getInstance()->setStencilEnable(m_pStencilParameter->enableStencilTest);
			if (m_pStencilParameter->enableStencilTest)
			{
				GLStateCache::getInstance()->setStencilFuncParameter(m_pStencilParameter->stencilFuncParam->func, m_pStencilParameter->stencilFuncParam->ref, m_pStencilParameter->stencilFuncParam->mask);
				GLStateCache::getInstance()->setStencilOpParameter(m_pStencilParameter->stencilOpParam->sfail, m_pStencilParameter->stencilOpParam->dpfail, m_pStencilParameter->stencilOpParam->dppass);
			}
		}
		else
		{
			// 否则，默认使用material的参数
			GLStateCache::getInstance()->setStencilEnable(enableStencilTest);
			if (enableStencilTest)
			{
				if (stencilFuncParam)
				{
					GLStateCache::getInstance()->setStencilFuncParameter(stencilFuncParam->func, stencilFuncParam->ref, stencilFuncParam->mask);
				}
				if (stencilOpParam)
				{
					GLStateCache::getInstance()->setStencilOpParameter(stencilOpParam->sfail, stencilOpParam->dpfail, stencilOpParam->dppass);
				}
			}
		}
		// blend
		if (m_pBlendParameter)
		{
			// 如果pass设置了blend相关参数
			GLStateCache::getInstance()->setBlendEnalbe(m_pBlendParameter->enableBlend);
			if (m_pBlendParameter->enableBlend)
			{
				GLStateCache::getInstance()->setBlendFuncParameter(m_pBlendParameter->blendFuncParam->sfactor, m_pBlendParameter->blendFuncParam->dfactor, m_pBlendParameter->blendFuncParam->safactor, m_pBlendParameter->blendFuncParam->dafactor);
				GLStateCache::getInstance()->setBlendEquation(m_pBlendParameter->blendEquation);
			}
		}
		else
		{
			// 否则，默认使用material的参数
			GLStateCache::getInstance()->setBlendEnalbe(enableBlend);
			if (enableBlend)
			{
				if (blendFuncParam)
				{
					GLStateCache::getInstance()->setBlendFuncParameter(blendFuncParam->sfactor, blendFuncParam->dfactor, blendFuncParam->safactor, blendFuncParam->dafactor);
				}
				GLStateCache::getInstance()->setBlendEquation(blendEquation);
			}
		}
	}

	void Pass::setEnableZTest(bool enableZTest)
	{
		if (!m_pZTestParameter)
		{
			m_pZTestParameter = std::make_shared<PassZTestParameter>();
		}
		m_pZTestParameter->enableZTest = enableZTest;
	}

	void Pass::setZTestFunc(GLenum ZTestFunc)
	{
		if (!m_pZTestParameter)
		{
			m_pZTestParameter = std::make_shared<PassZTestParameter>();
		}
		m_pZTestParameter->ZTestFunc = ZTestFunc;
	}

	void Pass::setZWrite(bool ZWrite)
	{
		if (!m_pZTestParameter)
		{
			m_pZTestParameter = std::make_shared<PassZTestParameter>();
		}
		m_pZTestParameter->ZWrite = ZWrite;
	}

	void Pass::setEnableStencilTest(bool enableStencilTest)
	{
		if (!m_pStencilParameter)
		{
			m_pStencilParameter = std::make_shared<PassStencilParameter>();
		}
		m_pStencilParameter->enableStencilTest = enableStencilTest;
	}

	void Pass::setStencilFuncParameter(GLint ref, GLuint mask)
	{
		if (!m_pStencilParameter)
		{
			m_pStencilParameter = std::make_shared<PassStencilParameter>();
		}
		m_pStencilParameter->stencilFuncParam->func = GL_EQUAL;
		m_pStencilParameter->stencilFuncParam->ref = ref;
		m_pStencilParameter->stencilFuncParam->mask = mask;
	}

	void Pass::setStencilFuncParameter(GLenum func, GLint ref, GLuint mask)
	{
		if (!m_pStencilParameter)
		{
			m_pStencilParameter = std::make_shared<PassStencilParameter>();
		}
		m_pStencilParameter->stencilFuncParam->func = func;
		m_pStencilParameter->stencilFuncParam->ref = ref;
		m_pStencilParameter->stencilFuncParam->mask = mask;
	}

	void Pass::setStencilOpParameter(GLenum sfail, GLenum dpfail, GLenum dppass)
	{
		if (!m_pStencilParameter)
		{
			m_pStencilParameter = std::make_shared<PassStencilParameter>();
		}
		m_pStencilParameter->stencilOpParam->sfail = sfail;
		m_pStencilParameter->stencilOpParam->dpfail = dpfail;
		m_pStencilParameter->stencilOpParam->dppass= dppass;
	}

	void Pass::setEnableBlend(bool enableBlend)
	{
		if (!m_pBlendParameter)
		{
			m_pBlendParameter = std::make_shared<PassBlendParameter>();
		}
		m_pBlendParameter->enableBlend = enableBlend;
	}

	void Pass::setBlendFuncParameter(GLenum sfactor, GLenum dfactor)
	{
		if (!m_pBlendParameter)
		{
			m_pBlendParameter = std::make_shared<PassBlendParameter>();
		}
		m_pBlendParameter->blendFuncParam->sfactor = sfactor;
		m_pBlendParameter->blendFuncParam->dfactor = dfactor;
		m_pBlendParameter->blendFuncParam->safactor = sfactor;
		m_pBlendParameter->blendFuncParam->dafactor = dfactor;
	}

	void Pass::setBlendFuncParameter(GLenum sfactor, GLenum dfactor, GLenum safactor, GLenum dafactor)
	{
		if (!m_pBlendParameter)
		{
			m_pBlendParameter = std::make_shared<PassBlendParameter>();
		}
		m_pBlendParameter->blendFuncParam->sfactor = sfactor;
		m_pBlendParameter->blendFuncParam->dfactor = dfactor;
		m_pBlendParameter->blendFuncParam->safactor = safactor;
		m_pBlendParameter->blendFuncParam->dafactor = dafactor;
	}

	void Pass::setBlendEquation(GLenum blendEquation)
	{
		if (!m_pBlendParameter)
		{
			m_pBlendParameter = std::make_shared<PassBlendParameter>();
		}
		m_pBlendParameter->blendEquation = blendEquation;
	}


}
