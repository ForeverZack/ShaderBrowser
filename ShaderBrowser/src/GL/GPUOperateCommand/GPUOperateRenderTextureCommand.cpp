#include "GPUOperateRenderTextureCommand.h"
#include "GL/GPUResource/Texture/RenderTexture.h"

namespace customGL
{
	GPUOperateRenderTextureCommand::GPUOperateRenderTextureCommand()
        : m_pRenderTexture(nullptr)
    {
        m_eCommandType = GOCT_RenderTexture;
        m_eOperateType = GOT_Update;
	}
    
	GPUOperateRenderTextureCommand::~GPUOperateRenderTextureCommand()
    {
        
    }
    
    void GPUOperateRenderTextureCommand::ready(GPUOperateType operateType)
    {
        BROWSER_ASSERT(m_pRenderTexture, "GPUOperateRenderTextureCommand does not have operate object, please check your program in function GPUOperateRenderTextureCommand::ready");
        
        BaseGPUOperateCommand::ready(operateType);
        
        // 逻辑线程调用，防止autorelease先执行
		m_pRenderTexture->retain();
    }
    
    void GPUOperateRenderTextureCommand::execute()
    {        
        switch(m_eOperateType)
        {
        case GPUOperateType::GOT_Create:
            {
                // 创建渲染纹理
				createRenderTexture();
            }
            break;
            
        case GPUOperateType::GOT_Update:
            {
                // 更新渲染纹理数据
				updateRenderTexture();
            }
            break;
                
        case GPUOperateType::GOT_Delete:
            {
                // 删除渲染纹理
				deleteRenderTexture();
            }
            break;
        }
        
    }
    
    // 结束执行 (渲染线程调用)
    void GPUOperateRenderTextureCommand::finish()
    {
        // 回收命令
        BaseGPUOperateCommand::finish();
        
        // 渲染线程调用，加到释放队列中去，队列会在逻辑线程中刷新释放
        AutoReleasePool::getInstance()->addReferenceFromRenderCore(m_pRenderTexture);
    }
    
    void GPUOperateRenderTextureCommand::createRenderTexture()
    {
		// 生成帧缓存
		glGenFramebuffers(1, &m_pRenderTexture->m_uFrameBufferId);
		// 更新帧缓存附件
		updateRenderTexture();

		// 资源加载完成
		m_pRenderTexture->m_eResouceState = GRS_Loaded;
    }
    
    void GPUOperateRenderTextureCommand::updateRenderTexture()
    {
		glBindFramebuffer(GL_FRAMEBUFFER, m_pRenderTexture->m_uFrameBufferId);

		for (auto itor = m_mAttachments->begin(); itor != m_mAttachments->end(); ++itor)
		{
			const GLuint& attachment = itor->first;
			RenderTextureAttachment& attachmentData = itor->second;

			// 缓冲是否没有创建
			if (attachmentData.buffer_object == 0)
			{
				switch (attachmentData.type)
				{
				case RenderTextureAttachmentType::RTAttachmentType_TBO:
					{
						// tbo
						glGenTextures(1, &attachmentData.buffer_object);
					}
					break;

				case RenderTextureAttachmentType::RTAttachmentType_RBO:
					{
						// rbo
						glGenRenderbuffers(1, &attachmentData.buffer_object);
					}
					break;
				}
			}

			switch (attachmentData.type)
			{
			case RenderTextureAttachmentType::RTAttachmentType_TBO:
				{
					// tbo
					glBindTexture(GL_TEXTURE_2D, attachmentData.buffer_object);
					// glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
					glTexImage2D(GL_TEXTURE_2D, 0, attachmentData.property1, m_pRenderTexture->getWidth(), m_pRenderTexture->getHeight(), 0, attachmentData.property2, attachmentData.property3, NULL);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, attachmentData.property4);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, attachmentData.property4);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, attachmentData.property5);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, attachmentData.property5);
					glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, attachmentData.buffer_object, 0);
				}
				break;

			case RenderTextureAttachmentType::RTAttachmentType_RBO:
				{
					// rbo
					glRenderbufferStorage(GL_RENDERBUFFER, attachmentData.property1, m_pRenderTexture->getWidth(), m_pRenderTexture->getHeight());
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, attachmentData.buffer_object);
				}
				break;
			}

		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    void GPUOperateRenderTextureCommand::deleteRenderTexture()
    {
		// 删除缓冲附件
		for (auto itor = m_mAttachments->begin(); itor != m_mAttachments->end(); ++itor)
		{
			const GLuint& attachment = itor->first;
			RenderTextureAttachment& attachmentData = itor->second;

			// 缓冲是否没有创建
			if (attachmentData.buffer_object == 0)
			{
				switch (attachmentData.type)
				{
				case RenderTextureAttachmentType::RTAttachmentType_TBO:
				{
					// tbo
					glDeleteTextures(1, &attachmentData.buffer_object);
				}
				break;

				case RenderTextureAttachmentType::RTAttachmentType_RBO:
				{
					// rbo
					glDeleteRenderbuffers(1, &attachmentData.buffer_object);
				}
				break;
				}
			}
		}
		// 删除帧缓存
		glDeleteFramebuffers(1, &m_pRenderTexture->m_uFrameBufferId);

		// 设置GPU删除标记
		m_pRenderTexture->setGPUDeleted(true);
    }
    


}
