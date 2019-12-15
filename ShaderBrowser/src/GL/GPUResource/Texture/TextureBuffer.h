#pragma once

#include <glad/glad.h>
#include "Common/Components/Reference.h"
#include "Common/Tools/Utils.h"
#include "GL/GPUResource/BaseGPUResource.h"

using namespace common;

namespace customGL
{
    class GPUOperateTextureBufferCommand;
    
	class TextureBuffer : public BaseGPUResource
	{
	public:
		static TextureBuffer* create(GLenum internalFormat = GL_RGBA32F);

	public:
		TextureBuffer(GLenum internalFormat);
		~TextureBuffer();
		friend class GPUOperateTextureBufferCommand;

    public:
        // 设置数据
        void setData(const std::vector<float>& data);
        void setData(const std::vector<glm::mat4>& data);

        REGISTER_PROPERTY_GET(unsigned int, m_uTextureId, TextureId);

	protected:
		// 创建gpu资源
		virtual void createGPUResource();
		// 更新gpu资源
//		virtual void updateGPUResource();
		// 删除gpu资源
		virtual void deleteGPUResource();
        // 更新gpu资源属性
//        void updateGPUResourceProperties();
       

	private:
		// 纹理
		GLuint m_uTextureId;
        // vbo
		GLuint m_uVBO;

		// 纹理format
		GLenum m_eInternalFormat;
	};
}

