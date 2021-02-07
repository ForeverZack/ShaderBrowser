#include "GPUOperateUniformBufferCommand.h"
#include "GL/GPUResource/Buffer/UniformBuffer.h"

namespace customGL
{
	GPUOperateUniformBufferCommand::GPUOperateUniformBufferCommand()
        : m_pBuffer(nullptr)
        , m_pData(nullptr)
		, m_uSize(0)
    {
        m_eCommandType = GOCT_UniformBuffer;
        m_eOperateType = GOT_Update;
	}
    
	GPUOperateUniformBufferCommand::~GPUOperateUniformBufferCommand()
    {
		BROWSER_SAFE_FREE_POINTER(m_pData);
    }
    
    void GPUOperateUniformBufferCommand::ready(GPUOperateType operateType)
    {
        BROWSER_ASSERT(m_pBuffer, "GPUOperateUniformBufferCommand does not have operate object, please check your program in function GPUOperateUniformBufferCommand::ready");
        
        BaseGPUOperateCommand::ready(operateType);
        
        // 逻辑线程调用，防止autorelease先执行
        m_pBuffer->retain();
    }
    
    void GPUOperateUniformBufferCommand::execute()
    {        
        switch(m_eOperateType)
        {
        case GPUOperateType::GOT_Create:
            {
                // 创建缓存
                createBuffer();
            }
            break;
            
        case GPUOperateType::GOT_Update:
            {
                // 更新缓存数据
                updateBuffer();
            }
            break;
                
        case GPUOperateType::GOT_Delete:
            {
                // 删除缓存
                deleteBuffer();
            }
            break;
        }
        
    }
    
    // 结束执行 (渲染线程调用)
    void GPUOperateUniformBufferCommand::finish()
    {
        // 清除
		m_pBuffer = nullptr;
        if (m_eCommandType == GPUOperateType::GOT_Update)
        {
			BROWSER_SAFE_FREE_POINTER(m_pData);
			m_uSize = 0;
		}
		m_pData = nullptr;
        
        // 回收命令
        BaseGPUOperateCommand::finish();
        
        // 渲染线程调用，加到释放队列中去，队列会在逻辑线程中刷新释放
        AutoReleasePool::getInstance()->addReferenceFromRenderCore(m_pBuffer);
    }
    
    void GPUOperateUniformBufferCommand::createBuffer()
    {
		glGenBuffers(1, &m_pBuffer->m_uVBO);
		glBindBuffer(GL_UNIFORM_BUFFER, m_pBuffer->m_uVBO);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// 资源加载完成
		m_pBuffer->m_eResouceState = GRS_Loaded;
    }
    
    void GPUOperateUniformBufferCommand::updateBuffer()
    {
        if (!m_pData || m_uSize<=0)
        {
            return;
        }
        
        glBindBuffer(GL_UNIFORM_BUFFER, m_pBuffer->m_uVBO);
        glBufferData(GL_UNIFORM_BUFFER, m_uSize, m_pData, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    
    void GPUOperateUniformBufferCommand::deleteBuffer()
    {
		if (m_pBuffer->m_uVBO != 0)
		{
			glDeleteBuffers(1, &m_pBuffer->m_uVBO);
		}

		// 设置GPU删除标记
		m_pBuffer->setGPUDeleted(true);
    }
    
    void GPUOperateUniformBufferCommand::setData(BufferData& data)
    {
		m_uSize = data.getRealSize();
		if (m_uSize > 0)
		{
			m_pData = malloc(m_uSize);
			memcpy(m_pData, data.getData(), m_uSize);
		}
    }
    
	void GPUOperateUniformBufferCommand::setData(const size_t size, const vector<string>& varnames, unordered_map<string, BufferData>& variables)
	{
		m_uSize = size;
		if (m_uSize > 0)
		{
			m_pData = malloc(m_uSize);
			size_t offset = 0;
			for (auto itor = varnames.begin(); itor != varnames.end(); ++itor)
			{
				const string& name = (*itor);
				BufferData& buffer = (variables.find(name))->second;
				memcpy((BYTE*)m_pData + offset, buffer.getData(), buffer.getRealSize());

				offset += buffer.getRealSize();
			}
		}
	}

}
