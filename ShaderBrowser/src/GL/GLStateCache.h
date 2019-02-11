#pragma once

#include "GL/GLProgram.h"
#include "Common/Tools/BaseSingleton.h"

using namespace common;

namespace customGL
{
    class GLStateCache : public BaseSingleton<GLStateCache>
	{
	public:
		GLStateCache();
		~GLStateCache();
        
    public:
        // 绑定纹理到opengl
        void bindTexture2DN(GLuint textureUnit, GLuint textureId);
        
        // 开启深度缓存测试
        void openDepthTest(GLenum depthFunc = GL_LESS, GLenum depthMask = GL_TRUE);
        // 关闭深度缓存测试
        void closeDepthTest();

    private:
        // textureId队列
        GLuint m_vTexIds[MAX_ACTIVE_TEXTURE];
        
	};
}

