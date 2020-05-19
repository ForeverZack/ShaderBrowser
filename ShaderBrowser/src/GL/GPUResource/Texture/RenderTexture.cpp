#include "RenderTexture.h"
#include "GL/GPUOperateCommand/GPUOperateCommandPool.h"
#include "GL/GPUOperateCommand/GPUOperateRenderTextureCommand.h"
#include "GL/System/GPUOperateSystem.h"

namespace customGL
{
	RenderTexture* create(unsigned int width, unsigned int height)
	{
		RenderTexture* textureBuffer = new RenderTexture(width, height);
		return textureBuffer;
	}

	RenderTexture* RenderTexture::create(unsigned int width, unsigned int height, const std::unordered_map<GLuint, RenderTextureAttachment>& attachments)
	{
		RenderTexture* textureBuffer = new RenderTexture(width, height, attachments);
		return textureBuffer;
	}


	RenderTexture::RenderTexture(unsigned int width, unsigned int height, const std::unordered_map<GLuint, RenderTextureAttachment>& attachments)
	{
		m_eResourceType = GPUResourceType::GRT_FrameBuffer;
		m_eResouceState = GRS_UnLoad; // default

		m_mAttachments = attachments;
		// ��gpu�ϴ���
		createGPUResource();
	}

	RenderTexture::RenderTexture(unsigned int width, unsigned int height, bool autoCreateGPURes/* = true*/)
		: m_uWidth(width)
		, m_uHeight(height)
		, m_uFrameBufferId(0)
	{
        m_eResourceType = GPUResourceType::GRT_FrameBuffer;
		m_eResouceState = GRS_UnLoad; // default

		// Ĭ�ϰ�һ����������
		m_mAttachments[GL_COLOR_ATTACHMENT0] = RenderTextureAttachment();
		
		if (autoCreateGPURes)
		{
			// ��gpu�ϴ���
			createGPUResource();
		}
	}

	RenderTexture::~RenderTexture()
	{
        deleteGPUResource();
    }

	void RenderTexture::useRenderTexture()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_uFrameBufferId);
	}

	void RenderTexture::setAttachment(GLuint attachment, const RenderTextureAttachment& data)
	{
		m_mAttachments[attachment] = data;

		// ����gpu��Դ
		updateGPUResource();
	}

	void RenderTexture::createGPUResource()
	{
        auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateRenderTextureCommand>(GPUOperateCommandType::GOCT_RenderTexture);
        cmd->setRenderTexture(this);
		cmd->setAttachments(m_mAttachments);
        cmd->ready(GPUOperateType::GOT_Create);
        GPUOperateSystem::getInstance()->addCommand(cmd);
	}

	void RenderTexture::updateGPUResource()
	{
		auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateRenderTextureCommand>(GPUOperateCommandType::GOCT_RenderTexture);
		cmd->setRenderTexture(this);
		cmd->setAttachments(m_mAttachments);
		cmd->ready(GPUOperateType::GOT_Update);
		GPUOperateSystem::getInstance()->addCommand(cmd);
	}

	void RenderTexture::deleteGPUResource()
	{
        BROWSER_ASSERT(m_eResouceState==GRS_Loaded, "RenderTexture state must be GRS_Loaded, then it can be destroyed on gpu");
        
        auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateRenderTextureCommand>(GPUOperateCommandType::GOCT_RenderTexture);
        cmd->setRenderTexture(this);
        cmd->ready(GPUOperateType::GOT_Delete);
        GPUOperateSystem::getInstance()->addCommand(cmd);
	}
    

}