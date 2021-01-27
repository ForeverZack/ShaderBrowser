#include "UniformBuffer.h"
#include "GL/GPUOperateCommand/GPUOperateCommandPool.h"
#include "GL/GPUOperateCommand/GPUOperateUniformBufferCommand.h"
#include "GL/System/GPUOperateSystem.h"

namespace customGL
{    
	UniformBuffer* UniformBuffer::create()
	{
		UniformBuffer* uniformBuffer = new UniformBuffer();
		return uniformBuffer;
	}

	UniformBuffer::UniformBuffer()
		: m_uVBO(0)
		, m_bGpuDataInit(false)
	{
        m_eResourceType = GPUResourceType::GRT_UniformBuffer;
//        m_eResouceState = GRS_UnLoad; // default

		createGPUResource();
	}

	UniformBuffer::~UniformBuffer()
	{
        deleteGPUResource();
    }

	void UniformBuffer::setData(const string& varname, const float& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, value, updateGpuData);
	}

	void UniformBuffer::setData(const string& varname, const glm::vec2& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, value, updateGpuData);
	}

	void UniformBuffer::setData(const string& varname, const glm::vec3& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, value, updateGpuData);
	}

	void UniformBuffer::setData(const string& varname, const glm::vec4& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, value, updateGpuData);
	}

	void UniformBuffer::setData(const string& varname, const int& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, value, updateGpuData);
	}

	void UniformBuffer::setData(const string& varname, const glm::ivec2& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, value, updateGpuData);
	}

	void UniformBuffer::setData(const string& varname, const glm::ivec3& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, value, updateGpuData);
	}

	void UniformBuffer::setData(const string& varname, const glm::ivec4& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, value, updateGpuData);
	}

	void UniformBuffer::setData(const string& varname, const bool& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, value, updateGpuData);
	}

	void UniformBuffer::setData(const string& varname, const glm::bvec2& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, value, updateGpuData);
	}

	void UniformBuffer::setData(const string& varname, const glm::bvec3& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, value, updateGpuData);
	}

	void UniformBuffer::setData(const string& varname, const glm::bvec4& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, value, updateGpuData);
	}

	void UniformBuffer::setData(const string& varname, const glm::mat2& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, value, updateGpuData);
	}

	void UniformBuffer::setData(const string& varname, const glm::mat2x3& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, value, updateGpuData);
	}

	void UniformBuffer::setData(const string& varname, const glm::mat2x4& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, value, updateGpuData);
	}

	void UniformBuffer::setData(const string& varname, const glm::mat3& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, value, updateGpuData);
	}

	void UniformBuffer::setData(const string& varname, const glm::mat3x2& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, value, updateGpuData);
	}

	void UniformBuffer::setData(const string& varname, const glm::mat3x4& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, value, updateGpuData);
	}

    
	void UniformBuffer::setData(const string& varname, const glm::mat4& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, value, updateGpuData);
    }

	void UniformBuffer::setData(const string& varname, const glm::mat4x2& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, value, updateGpuData);
	}

	void UniformBuffer::setData(const string& varname, const glm::mat4x3& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, value, updateGpuData);
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
		// cmd->setData(data);
		// cmd->ready(GPUOperateType::GOT_Update);
		// GPUOperateSystem::getInstance()->addCommand(cmd);
	}

	void UniformBuffer::deleteGPUResource()
	{
        //BROWSER_ASSERT(m_eResouceState==GRS_Loaded, "UniformBuffer state must be GRS_Loaded, then it can be destroyed on gpu");
        
        // auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateTextureBufferCommand>(GPUOperateCommandType::GOCT_TextureBuffer);
        // cmd->setTextureBuffer(this);
        // cmd->ready(GPUOperateType::GOT_Delete);
        // GPUOperateSystem::getInstance()->addCommand(cmd);
	}
    

}
