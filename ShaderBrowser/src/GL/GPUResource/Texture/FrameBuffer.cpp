#include "FrameBuffer.h"
#include "GL/GPUOperateCommand/GPUOperateCommandPool.h"
#include "GL/GPUOperateCommand/GPUOperateTextureBufferCommand.h"
#include "GL/System/GPUOperateSystem.h"

namespace customGL
{
	FrameBuffer* FrameBuffer::create()
	{
		FrameBuffer* textureBuffer = new FrameBuffer();
		return textureBuffer;
	}

	FrameBuffer::FrameBuffer(GLenum)
		: m_uTextureId(0)
		, m_uVBO(0)
	{
        m_eResourceType = GPUResourceType::GRT_FrameBuffer;
//        m_eResouceState = GRS_UnLoad; // default

		createGPUResource();
	}

	FrameBuffer::~FrameBuffer()
	{
        deleteGPUResource();
    }
    
    

	void FrameBuffer::createGPUResource()
	{
        //auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateTextureBufferCommand>(GPUOperateCommandType::GOCT_TextureBuffer);
        //cmd->setTextureBuffer(this);
        //cmd->ready(GPUOperateType::GOT_Create);
        //GPUOperateSystem::getInstance()->addCommand(cmd);
	}

	void FrameBuffer::updateGPUResource()
	{

	}

	void FrameBuffer::deleteGPUResource()
	{
        //BROWSER_ASSERT(m_eResouceState==GRS_Loaded, "FrameBuffer state must be GRS_Loaded, then it can be destroyed on gpu");
        //
        //auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateTextureBufferCommand>(GPUOperateCommandType::GOCT_TextureBuffer);
        //cmd->setTextureBuffer(this);
        //cmd->ready(GPUOperateType::GOT_Delete);
        //GPUOperateSystem::getInstance()->addCommand(cmd);
	}
    

}
