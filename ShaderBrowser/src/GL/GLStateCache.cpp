#include "GLStateCache.h"
#include "Common/Tools/Utils.h"

namespace customGL
{    

    GLStateCache::GLStateCache()
		// depth test
		: m_bEnableZTest(false)
		, m_eZTestFunc(GL_NONE)
		, m_bZWrite(false)
		, m_bEnableStencilTest(false)
		, m_bEnableBlend(false)
		, m_eBlendEquation(GL_FUNC_ADD)
    {

    }
    
    GLStateCache::~GLStateCache()
    {
        
    }

	void GLStateCache::update(float deltaTime)
	{
		// 重置texturenId队列
		for (int i = 0; i < MAX_ACTIVE_TEXTURE; ++i)
		{
			m_vTexIds[i] = 0;
		}
	}
    
    void GLStateCache::bindTexture2DN(GLuint textureUnit, GLuint textureId)
    {
        if (m_vTexIds[textureUnit] != textureId)
        {
            m_vTexIds[textureUnit] = textureId;
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, textureId);
        }
    }
    
    void GLStateCache::bindSamplerBuffer(GLuint textureUnit, GLuint textureId)
    {
        if (m_vTexIds[textureUnit] != textureId)
        {
            m_vTexIds[textureUnit] = textureId;
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_BUFFER, textureId);
        }
    }

	void GLStateCache::bindImageBuffer(GLuint textureUnit, GLuint textureId, GLenum access, GLenum format)
	{
		if (m_vTexIds[textureUnit] != textureId)
		{
            //void (*aaa)(GLuint, GLuint, GLint, GLboolean, GLint, GLenum, GLenum) = glBindImageTexture;	// mac上不支持opengl4.3，所以根本没有glBindImageTexture
            glBindImageTexture(textureUnit, textureId, 0, GL_FALSE, 0, access, format);
		}
	}
    
    void GLStateCache::setZTest(bool enable, GLenum depthFunc/* = GL_LESS*/, GLenum zwrite/* = GL_TRUE*/)
    {
        // 深度测试开关
		if (m_bEnableZTest != enable)
		{
			m_bEnableZTest = enable;
			if (enable)
			{
				glEnable(GL_DEPTH_TEST);
			}
			else
			{
				glDisable(GL_DEPTH_TEST);
			}
		}

		if (enable)
		{
			// 深度测试方法
			if (m_eZTestFunc != depthFunc)
			{
				m_eZTestFunc = depthFunc;
				glDepthFunc(depthFunc);     // 默认值:深度小于当前深度缓存值时可通过深度测试
			}
			// 深度缓存写入开关
			if (m_bZWrite != zwrite)
			{
				m_bZWrite = zwrite;
				glDepthMask(zwrite);     // 默认值:表示深度缓存可写,如果为GL_FALSE则表示深度缓存为只读
			}
		}
    }

	void GLStateCache::setStencilEnable(bool enable)
	{
		if (m_bEnableStencilTest != enable)
		{
			m_bEnableStencilTest = enable;
			if (enable)
			{
				glEnable(GL_STENCIL_TEST);
			}
			else
			{
				glDisable(GL_STENCIL_TEST);
			}
		}
	}

	void GLStateCache::setStencilFuncParameter(GLenum func, GLint ref, GLuint mask)
	{
		if (m_oStencilFuncParam.func!=func || m_oStencilFuncParam.ref!=ref || m_oStencilFuncParam.mask!=mask)
		{
			m_oStencilFuncParam.func = func;
			m_oStencilFuncParam.ref = ref;
			m_oStencilFuncParam.mask = mask;

			glStencilFunc(func, ref, mask);
		}
	}

	void GLStateCache::setStencilOpParameter(GLenum sfail, GLenum dpfail, GLenum dppass)
	{
		if (m_oStencilOpParam.sfail!=sfail || m_oStencilOpParam.dpfail!=dpfail || m_oStencilOpParam.dppass!=dppass)
		{
			m_oStencilOpParam.sfail = sfail;
			m_oStencilOpParam.dpfail = dpfail;
			m_oStencilOpParam.dppass = dppass;

			glStencilOp(sfail, dpfail, dppass);
		}
	}

	void GLStateCache::setBlendEnalbe(bool enable)
	{
		if (m_bEnableBlend != enable)
		{
			m_bEnableBlend = enable;
			if (enable)
			{
				glEnable(GL_BLEND);
			}
			else
			{
				glDisable(GL_BLEND);
			}
		}
	}

	void GLStateCache::setBlendFuncParameter(GLenum sfactor, GLenum dfactor, GLenum safactor, GLenum dafactor)
	{
		if (m_oBlendFuncParam.sfactor!=sfactor || m_oBlendFuncParam.dfactor!=dfactor || m_oBlendFuncParam.safactor!=safactor || m_oBlendFuncParam.dafactor!=dafactor)
		{
			m_oBlendFuncParam.sfactor = sfactor;
			m_oBlendFuncParam.dfactor = dfactor;
			m_oBlendFuncParam.safactor = safactor;
			m_oBlendFuncParam.dafactor = dafactor;

			glBlendFuncSeparate(sfactor, dfactor, safactor, dafactor);
		}
	}

	void GLStateCache::setBlendEquation(GLenum equation)
	{
		if (m_eBlendEquation != equation)
		{
			m_eBlendEquation = equation;

			glBlendEquation(equation);
		}
	}
    
    
}
