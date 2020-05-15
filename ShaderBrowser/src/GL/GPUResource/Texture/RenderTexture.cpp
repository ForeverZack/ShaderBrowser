#include "RenderTexture.h"
#include "GL/GPUOperateCommand/GPUOperateCommandPool.h"
#include "GL/GPUOperateCommand/GPUOperateRenderTextureCommand.h"
#include "GL/System/GPUOperateSystem.h"

namespace customGL
{
	RenderTexture* RenderTexture::create(unsigned int width, unsigned int height)
	{
		RenderTexture* textureBuffer = new RenderTexture(width, height);
		return textureBuffer;
	}

	RenderTexture::RenderTexture(unsigned int width, unsigned int height)
		: m_uWidth(width)
		, m_uHeight(height)
		, m_uFrameBufferId(0)
	{
        m_eResourceType = GPUResourceType::GRT_FrameBuffer;
//        m_eResouceState = GRS_UnLoad; // default

		// Ĭ�ϰ�һ��������
		m_mAttachments[GL_COLOR_ATTACHMENT0] = RenderTextureAttachment();
		// ��gpu�ϴ���
		createGPUResource();
	}

	RenderTexture::~RenderTexture()
	{
        deleteGPUResource();
    }
    

	void RenderTexture::createGPUResource()
	{
        //auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateRenderTextureCommand>(GPUOperateCommandType::GOCT_RenderTexture);
        //cmd->setTextureBuffer(this);
        //cmd->ready(GPUOperateType::GOT_Create);
        //GPUOperateSystem::getInstance()->addCommand(cmd);
	}

	void RenderTexture::updateGPUResource()
	{

	}

	void RenderTexture::deleteGPUResource()
	{
        //BROWSER_ASSERT(m_eResouceState==GRS_Loaded, "RenderTexture state must be GRS_Loaded, then it can be destroyed on gpu");
        //
        //auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateTextureBufferCommand>(GPUOperateCommandType::GOCT_TextureBuffer);
        //cmd->setTextureBuffer(this);
        //cmd->ready(GPUOperateType::GOT_Delete);
        //GPUOperateSystem::getInstance()->addCommand(cmd);
	}
    

}
