#include "TextureBuffer.h"
#include "GL/GPUOperateCommand/GPUOperateCommandPool.h"
#include "GL/GPUOperateCommand/GPUOperateTexture2DCommand.h"
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
    }

	void TextureBuffer::createGPUResource()
	{
        BROWSER_ASSERT(m_eResouceState==GRS_DataLoaded, "TextureBuffer state must be GRS_DataLoaded, then it can be created on gpu");
                       
        //auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateTexture2DCommand>(GPUOperateCommandType::GOCT_Texture2D);
        //cmd->setTexture(this);
        //cmd->setImage(m_oImage);
        //cmd->setTexParameters(m_eWrapTypeS, m_eWrapTypeT, m_eFilterMin, m_eFilterMag);
        //cmd->ready(GPUOperateType::GOT_Create);
        //GPUOperateSystem::getInstance()->addCommand(cmd);
	}

	void TextureBuffer::updateGPUResource()
	{
		//auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateTexture2DCommand>(GPUOperateCommandType::GOCT_Texture2D);
		//cmd->setTexture(this);
		//cmd->setImage(m_oImage);
		//cmd->ready(GPUOperateType::GOT_Update);
		//GPUOperateSystem::getInstance()->addCommand(cmd);
	}

	void TextureBuffer::deleteGPUResource()
	{
        BROWSER_ASSERT(m_eResouceState==GRS_Loaded, "Texture2D state must be GRS_Loaded, then it can be destroyed on gpu");
        
        //auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateTexture2DCommand>(GPUOperateCommandType::GOCT_Texture2D);
        //cmd->setTexture(this);
        //cmd->ready(GPUOperateType::GOT_Delete);
        //GPUOperateSystem::getInstance()->addCommand(cmd);
	}

    void TextureBuffer::updateGPUResourceProperties()
    {
        // GPU命令
        //auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateTexture2DCommand>(GPUOperateCommandType::GOCT_Texture2D);
        //cmd->setTexture(this);
        //cmd->setTexParameters(m_eWrapTypeS, m_eWrapTypeT, m_eFilterMin, m_eFilterMag);
        //cmd->ready(GPUOperateType::GOT_UpdateProperties_1);
        //GPUOperateSystem::getInstance()->addCommand(cmd);
    }
    

}
