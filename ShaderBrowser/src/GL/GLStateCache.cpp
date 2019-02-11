#include "GLStateCache.h"
#include "Common/Tools/Utils.h"

namespace customGL
{    

    GLStateCache::GLStateCache()
    {

    }
    
    GLStateCache::~GLStateCache()
    {
        
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
    
    void GLStateCache::openDepthTest(GLenum depthFunc/* = GL_LESS*/, GLenum depthMask/* = GL_TRUE*/)
    {
        // 开启深度测试
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(depthFunc);     // 默认值:深度小于当前深度缓存值时可通过深度测试
        glDepthMask(depthMask);     // 默认值:表示深度缓存可写,如果为GL_FALSE则表示深度缓存为只读
    }
    
    void GLStateCache::closeDepthTest()
    {
        // 关闭深度缓存测试
        glDisable(GL_DEPTH_TEST);
    }
    
    
}
