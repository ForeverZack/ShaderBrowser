#include "GPUOperateTextureBufferCommand.h"
#include "GL/GPUResource/Texture/TextureBuffer.h"

namespace customGL
{
	GPUOperateTextureBufferCommand::GPUOperateTextureBufferCommand()
        : m_pTexture(nullptr)
		, m_eInternalFormat(GL_RGBA32F)
        , m_pData(nullptr)
        , m_uSize(0)
    {
        m_eCommandType = GOCT_TextureBuffer;
        m_eOperateType = GOT_Update;
	}
    
	GPUOperateTextureBufferCommand::~GPUOperateTextureBufferCommand()
    {
        
    }
    
    void GPUOperateTextureBufferCommand::ready(GPUOperateType operateType)
    {
        BROWSER_ASSERT(m_pTexture, "GPUOperateTextureBufferCommand does not have operate object, please check your program in function GPUOperateTextureBufferCommand::ready");
        
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
        // 清除
		m_pTexture = nullptr;
        m_pData = nullptr;
        m_uSize = 0;
        if (m_eCommandType == GPUOperateType::GOT_Update)
        {
            std::vector<float> vec;
            m_uValue.val_float.swap(vec);
        }
        
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

		// 资源加载完成
		m_pTexture->m_eResouceState = GRS_Loaded;
    }
    
    void GPUOperateTextureBufferCommand::updateTextureBuffer()
    {
        if (!m_pData || m_uSize<=0)
        {
            return;
        }
        
        glBindBuffer(GL_TEXTURE_BUFFER, m_pTexture->m_uVBO);
        glBufferData(GL_TEXTURE_BUFFER, m_uSize, m_pData, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_TEXTURE_BUFFER, 0);
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
    
    void GPUOperateTextureBufferCommand::setData(const std::vector<glm::mat4>& data)
    {
        // (glm::mat4)会按照列主序的顺序传入，即第一列第二列第三列
        m_uValue.val_mat4 = data;
        if (data.size() > 0)
        {
            m_pData = &m_uValue.val_mat4[0];
            m_uSize = sizeof(glm::mat4) * data.size();
        }
        else
        {
            m_pData = nullptr;
            m_uSize = 0;
        }
    }
    
    void GPUOperateTextureBufferCommand::setData(const std::vector<float>& data)
    {
        m_uValue.val_float = data;
        if (data.size() > 0)
        {
            m_pData = &m_uValue.val_float[0];
            m_uSize = sizeof(float) * data.size();
        }
        else
        {
            m_pData = nullptr;
            m_uSize = 0;
        }
    }
    

}
