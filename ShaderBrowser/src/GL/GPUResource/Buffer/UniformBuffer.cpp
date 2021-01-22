#include "UniformBuffer.h"
#include "GL/GPUOperateCommand/GPUOperateCommandPool.h"
#include "GL/GPUOperateCommand/GPUOperateTextureBufferCommand.h"
#include "GL/System/GPUOperateSystem.h"

namespace customGL
{
	UniformBuffer* UniformBuffer::create()
	{
		UniformBuffer* uniformBuffer = new UniformBuffer(internalFormat);
		return uniformBuffer;
	}

	UniformBuffer::UniformBuffer()
		: m_uVBO(0)
	{
        m_eResourceType = GPUResourceType::GRT_UniformBuffer;
//        m_eResouceState = GRS_UnLoad; // default

		createGPUResource();
	}

	UniformBuffer::~UniformBuffer()
	{
        deleteGPUResource();
    }
    
    void UniformBuffer::setData(const std::vector<float>& data)
    {
        // auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateTextureBufferCommand>(GPUOperateCommandType::GOCT_TextureBuffer);
        // cmd->setTextureBuffer(this);
        // cmd->setData(data);
        // cmd->ready(GPUOperateType::GOT_Update);
        // GPUOperateSystem::getInstance()->addCommand(cmd);
    }
    
    void UniformBuffer::setData(const std::vector<glm::mat4>& data)
    {
        // auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateTextureBufferCommand>(GPUOperateCommandType::GOCT_TextureBuffer);
        // cmd->setTextureBuffer(this);
        // cmd->setData(data);
        // cmd->ready(GPUOperateType::GOT_Update);
        // GPUOperateSystem::getInstance()->addCommand(cmd);
    }

	void UniformBuffer::createGPUResource()
	{
        // auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateTextureBufferCommand>(GPUOperateCommandType::GOCT_TextureBuffer);
        // cmd->setTextureBuffer(this);
        // cmd->ready(GPUOperateType::GOT_Create);
        // GPUOperateSystem::getInstance()->addCommand(cmd);
	}
	
	void UniformBuffer::updateGPUResource()
	{
		// auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateTextureBufferCommand>(GPUOperateCommandType::GOCT_TextureBuffer);
		// cmd->setTextureBuffer(this);
		// cmd->setData(m_vData);
		// cmd->ready(GPUOperateType::GOT_Update);
		// GPUOperateSystem::getInstance()->addCommand(cmd);
	}

	void UniformBuffer::deleteGPUResource()
	{
        BROWSER_ASSERT(m_eResouceState==GRS_Loaded, "UniformBuffer state must be GRS_Loaded, then it can be destroyed on gpu");
        
        // auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateTextureBufferCommand>(GPUOperateCommandType::GOCT_TextureBuffer);
        // cmd->setTextureBuffer(this);
        // cmd->ready(GPUOperateType::GOT_Delete);
        // GPUOperateSystem::getInstance()->addCommand(cmd);
	}
    

}
