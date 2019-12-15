#include "TextureBuffer.h"
#include "GL/GPUOperateCommand/GPUOperateCommandPool.h"
#include "GL/GPUOperateCommand/GPUOperateTextureBufferCommand.h"
#include "GL/System/GPUOperateSystem.h"

namespace customGL
{
	TextureBuffer* TextureBuffer::create(GLenum internalFormat/* = GL_RGBA32F*/)
	{
		TextureBuffer* textureBuffer = new TextureBuffer(internalFormat);
		return textureBuffer;
	}

	TextureBuffer::TextureBuffer(GLenum internalFormat)
		: m_uTextureId(0)
		, m_uVBO(0)
		, m_eInternalFormat(internalFormat)
	{
        m_eResourceType = GPUResourceType::GRT_TextureBuffer;
//        m_eResouceState = GRS_UnLoad; // default

		createGPUResource();
	}

	TextureBuffer::~TextureBuffer()
	{
        deleteGPUResource();
    }
    
    void TextureBuffer::setData(const std::vector<float>& data)
    {
        auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateTextureBufferCommand>(GPUOperateCommandType::GOCT_TextureBuffer);
        cmd->setTextureBuffer(this);
        cmd->setData(data);
        cmd->ready(GPUOperateType::GOT_Update);
        GPUOperateSystem::getInstance()->addCommand(cmd);
    }
    
    void TextureBuffer::setData(const std::vector<glm::mat4>& data)
    {
        auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateTextureBufferCommand>(GPUOperateCommandType::GOCT_TextureBuffer);
        cmd->setTextureBuffer(this);
        cmd->setData(data);
        cmd->ready(GPUOperateType::GOT_Update);
        GPUOperateSystem::getInstance()->addCommand(cmd);
    }

	void TextureBuffer::createGPUResource()
	{
        auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateTextureBufferCommand>(GPUOperateCommandType::GOCT_TextureBuffer);
        cmd->setTextureBuffer(this);
        cmd->ready(GPUOperateType::GOT_Create);
        GPUOperateSystem::getInstance()->addCommand(cmd);
	}

	void TextureBuffer::deleteGPUResource()
	{
        BROWSER_ASSERT(m_eResouceState==GRS_Loaded, "TextureBuffer state must be GRS_Loaded, then it can be destroyed on gpu");
        
        auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateTextureBufferCommand>(GPUOperateCommandType::GOCT_TextureBuffer);
        cmd->setTextureBuffer(this);
        cmd->ready(GPUOperateType::GOT_Delete);
        GPUOperateSystem::getInstance()->addCommand(cmd);
	}

//    void TextureBuffer::updateGPUResourceProperties()
//    {
//
//    }
    

}
