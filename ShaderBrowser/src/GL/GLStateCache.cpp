#include "GLStateCache.h"
#include "Common/Tools/Utils.h"

namespace customGL
{    

    GLStateCache::GLStateCache()
		// depth test
		: m_bEnableZTest(false)
		, m_eZTestFunc(GL_NONE)
		, m_bZWrite(false)
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
    
    void GLStateCache::setZTest(bool enable, GLenum depthFunc/* = GL_LESS*/, GLenum depthMask/* = GL_TRUE*/)
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
			if (m_bZWrite != depthMask)
			{
				m_bZWrite = depthMask;
				glDepthMask(depthMask);     // 默认值:表示深度缓存可写,如果为GL_FALSE则表示深度缓存为只读
			}
		}
    }
    
    
}
