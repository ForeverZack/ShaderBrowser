#pragma once

#include "GL/GPUResource/Shader/GLProgram.h"
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
		// 刷新 (这里刷新，主要是因为引用了imgui插件，其中绑定了字符集纹理，会破坏本系统的逻辑，所以每帧更新一次，清空记录的纹理)
		void update(float deltaTime);
        // 绑定纹理到opengl
        void bindTexture2DN(GLuint textureUnit, GLuint textureId);
		void bindSamplerBuffer(GLuint textureUnit, GLuint textureId);
		void bindImageBuffer(GLuint textureUnit, GLuint textureId, GLenum access, GLenum format);
        
        // 设置深度缓存测试
        void setZTest(bool enable, GLenum depthFunc = GL_LESS, GLenum depthMask = GL_TRUE);


    private:
        // textureId队列
        GLuint m_vTexIds[MAX_ACTIVE_TEXTURE];

		// depth
		// 深度测试开关		开启：glEnable(GL_DEPTH_TEST);		关闭：glDisable(GL_DEPTH_TEST);
		bool m_bEnableZTest;
		// 深度测试方法		glDepthFunc(GL_LESS);
		/*
		函数						描述
		GL_ALWAYS			永远通过深度测试
		GL_NEVER			永远不通过深度测试
		GL_LESS				在片段深度值小于缓冲的深度值时通过测试
		GL_EQUAL			在片段深度值等于缓冲区的深度值时通过测试
		GL_LEQUAL			在片段深度值小于等于缓冲区的深度值时通过测试
		GL_GREATER		在片段深度值大于缓冲区的深度值时通过测试
		GL_NOTEQUAL	在片段深度值不等于缓冲区的深度值时通过测试
		GL_GEQUAL			在片段深度值大于等于缓冲区的深度值时通过测试
		*/
		GLenum m_eZTestFunc;
		// 深度写入		开启：glDepthMask(GL_TRUE);	关闭：glDepthMask(GL_FALSE);
		bool m_bZWrite;
        
	};
}

