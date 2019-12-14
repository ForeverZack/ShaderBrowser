#include "GPUOperateTextureBufferCommand.h"
#include "GL/GPUResource/Texture/TextureBuffer.h"

namespace customGL
{
	GPUOperateTextureBufferCommand::GPUOperateTextureBufferCommand()
        : m_pTexture(nullptr)
		, m_eInternalFormat(GL_RGBA32F)
    {
        m_eCommandType = GOCT_Texture2D;
        m_eOperateType = GOT_Update;
	}
    
	GPUOperateTextureBufferCommand::~GPUOperateTextureBufferCommand()
    {
        
    }
    
    void GPUOperateTextureBufferCommand::ready(GPUOperateType operateType)
    {
        BROWSER_ASSERT(m_pTexture, "GPUOperateTexture2DCommand does not have operate object, please check your program in function GPUOperateTexture2DCommand::ready");
        
        BaseGPUOperateCommand::ready(operateType);
        
        // 逻辑线程调用，防止autorelease先执行
        m_pTexture->retain();
    }
    
    void GPUOperateTextureBufferCommand::execute()
    {        
        switch(m_eOperateType)
        {
        case GPUOperateType::GOT_Create:
            {
                // 创建纹理
                createTextureBuffer();
            }
            break;
            
        case GPUOperateType::GOT_Update:
            {
                // 更新纹理数据
                updateTextureBuffer();
            }
            break;
                
        case GPUOperateType::GOT_Delete:
            {
                // 删除纹理
                deleteTextureBuffer();
            }
            break;
        }
        
    }
    
    // 结束执行 (渲染线程调用)
    void GPUOperateTextureBufferCommand::finish()
    {
        // 回收命令
        BaseGPUOperateCommand::finish();
        
        // 渲染线程调用，加到释放队列中去，队列会在逻辑线程中刷新释放
        AutoReleasePool::getInstance()->addReferenceFromRenderCore(m_pTexture);
    }
    
    void GPUOperateTextureBufferCommand::createTextureBuffer()
    {
		glGenTextures(1, &m_pTexture->m_uTextureId);
		glGenBuffers(1, &m_pTexture->m_uVBO);

		glBindBuffer(GL_TEXTURE_BUFFER, m_pTexture->m_uVBO);
		glBindTexture(GL_TEXTURE_BUFFER, m_pTexture->m_uTextureId);
		glTexBuffer(GL_TEXTURE_BUFFER, m_eInternalFormat, m_pTexture->m_uVBO);
		glBindTexture(GL_TEXTURE_BUFFER, 0);
		glBindBuffer(GL_TEXTURE_BUFFER, 0);
    }
    
    void GPUOperateTextureBufferCommand::updateTextureBuffer()
    {
      
    }
    
    void GPUOperateTextureBufferCommand::deleteTextureBuffer()
    {
        if (m_pTexture->m_uTextureId != 0)
        {
            glDeleteTextures(1, &m_pTexture->m_uTextureId);
        }
		if (m_pTexture->m_uVBO != 0)
		{
			glDeleteBuffers(1, &m_pTexture->m_uVBO);
		}

		// 设置GPU删除标记
		m_pTexture->setGPUDeleted(true);
    }
    

}
