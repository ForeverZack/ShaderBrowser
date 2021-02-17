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
		, m_uSize(0)
	{
        m_eResourceType = GPUResourceType::GRT_UniformBuffer;
//        m_eResouceState = GRS_UnLoad; // default

		createGPUResource();
	}

	UniformBuffer::~UniformBuffer()
	{
        deleteGPUResource();
    }

	void UniformBuffer::addVariable(const string& varname, const size_t elementSize, const size_t length)
	{
		BROWSER_ASSERT(m_mVariableDatas.find(varname) == m_mVariableDatas.end(), "UniformBuffer's variable has already added!");

		BufferData buffer;
		buffer.initData(elementSize, length);

		m_vVariableNames.push_back(varname);
		m_mVariableDatas.emplace(varname, std::move(buffer));
	}

	void UniformBuffer::setData(const string& varname, float& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, &value, sizeof(float));

		if (updateGpuData)
		{
			updateGPUResource();
		}
	}

	void UniformBuffer::setData(const string& varname, glm::vec2& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, &value, sizeof(glm::vec2));

		if (updateGpuData)
		{
			updateGPUResource();
		}
	}

	void UniformBuffer::setData(const string& varname, glm::vec3& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, &value, sizeof(glm::vec3));

		if (updateGpuData)
		{
			updateGPUResource();
		}
	}

	void UniformBuffer::setData(const string& varname, glm::vec4& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, &value, sizeof(glm::vec4));

		if (updateGpuData)
		{
			updateGPUResource();
		}
	}

	void UniformBuffer::setData(const string& varname, int& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, &value, sizeof(int));

		if (updateGpuData)
		{
			updateGPUResource();
		}
	}

	void UniformBuffer::setData(const string& varname, glm::ivec2& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, &value, sizeof(glm::ivec2));

		if (updateGpuData)
		{
			updateGPUResource();
		}
	}

	void UniformBuffer::setData(const string& varname, glm::ivec3& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, &value, sizeof(glm::ivec3));

		if (updateGpuData)
		{
			updateGPUResource();
		}
	}

	void UniformBuffer::setData(const string& varname, glm::ivec4& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, &value, sizeof(glm::ivec4));

		if (updateGpuData)
		{
			updateGPUResource();
		}
	}

	void UniformBuffer::setData(const string& varname, bool& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, &value, sizeof(bool));

		if (updateGpuData)
		{
			updateGPUResource();
		}
	}

	void UniformBuffer::setData(const string& varname, const glm::mat2& value, bool updateGpuData/* = true*/)
	{
		// align: mat2 -> mat2x4
        glm::mat2x4 temp = glm::mat2x4(value);
		setBufferData(varname, &temp, sizeof(glm::mat2x4));

		if (updateGpuData)
		{
			updateGPUResource();
		}
	}

	void UniformBuffer::setData(const string& varname, const glm::mat2x3& value, bool updateGpuData/* = true*/)
	{
		// align: mat2x3 -> mat2x4
        glm::mat2x4 temp = glm::mat2x4(value);
		setBufferData(varname, &temp, sizeof(glm::mat2x4));

		if (updateGpuData)
		{
			updateGPUResource();
		}
	}

	void UniformBuffer::setData(const string& varname, glm::mat2x4& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, &value, sizeof(glm::mat2x4));

		if (updateGpuData)
		{
			updateGPUResource();
		}
	}

	void UniformBuffer::setData(const string& varname, const glm::mat3& value, bool updateGpuData/* = true*/)
	{
		// align: mat3 -> mat3x4
        glm::mat3x4 temp = glm::mat3x4(value);
		setBufferData(varname, &temp, sizeof(glm::mat3x4));

		if (updateGpuData)
		{
			updateGPUResource();
		}
	}

	void UniformBuffer::setData(const string& varname, const glm::mat3x2& value, bool updateGpuData/* = true*/)
	{
		// align: mat3x2 -> mat3x4
        glm::mat3x4 temp = glm::mat3x4(value);
		setBufferData(varname, &temp, sizeof(glm::mat3x4));

		if (updateGpuData)
		{
			updateGPUResource();
		}
	}

	void UniformBuffer::setData(const string& varname, glm::mat3x4& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, &value, sizeof(glm::mat3x4));

		if (updateGpuData)
		{
			updateGPUResource();
		}
	}

	void UniformBuffer::setData(const string& varname, const glm::mat4x2& value, bool updateGpuData/* = true*/)
	{
		// align: mat4x2 -> mat4
        glm::mat4 temp = glm::mat4(value);
		setBufferData(varname, &temp, sizeof(glm::mat4));

		if (updateGpuData)
		{
			updateGPUResource();
		}
	}

	void UniformBuffer::setData(const string& varname, const glm::mat4x3& value, bool updateGpuData/* = true*/)
	{
		// align: mat4x3 -> mat4
        glm::mat4 temp = glm::mat4(value);
		setBufferData(varname, &temp, sizeof(glm::mat4));

		if (updateGpuData)
		{
			updateGPUResource();
		}
	}

	void UniformBuffer::setData(const string& varname, glm::mat4& value, bool updateGpuData/* = true*/)
	{
		setBufferData(varname, &value, sizeof(glm::mat4));

		if (updateGpuData)
		{
			updateGPUResource();
		}
	}
    
    void UniformBuffer::setData(const string& varname, std::vector<glm::vec2>& value, bool updateGpuData/* = true*/)
    {
        auto itor = m_mVariableDatas.find(varname);
        BROWSER_ASSERT(itor != m_mVariableDatas.end(), "UniformBuffer's variable has not been added !");
        
        BufferData& bufferData = itor->second;
        for (size_t i=0; i<value.size(); ++i)
        {
            setVectorBufferData(bufferData, &value[i], i, sizeof(glm::vec2));
        }
        
        if (updateGpuData)
        {
            updateGPUResource();
        }
    }
    
    void UniformBuffer::setData(const string& varname, std::vector<glm::vec3>& value, bool updateGpuData/* = true*/)
    {
        auto itor = m_mVariableDatas.find(varname);
        BROWSER_ASSERT(itor != m_mVariableDatas.end(), "UniformBuffer's variable has not been added !");
        
        BufferData& bufferData = itor->second;
        for (size_t i=0; i<value.size(); ++i)
        {
            setVectorBufferData(bufferData, &value[i], i, sizeof(glm::vec3));
        }
        
        if (updateGpuData)
        {
            updateGPUResource();
        }
    }
    
    void UniformBuffer::setData(const string& varname, std::vector<glm::vec4>& value, bool updateGpuData/* = true*/)
    {
        // vec4类型可以简写
        setBufferData(varname, &value, sizeof(glm::vec4)*value.size());
        
        if (updateGpuData)
        {
            updateGPUResource();
        }
    }
    
    void UniformBuffer::setData(const string& varname, std::vector<glm::mat4x2>& value, bool updateGpuData/* = true*/)
    {
        // mat4x2类型可以简写
        setBufferData(varname, &value, sizeof(glm::mat4x2)*value.size());
        
        if (updateGpuData)
        {
            updateGPUResource();
        }
    }
    
    void UniformBuffer::setData(const string& varname, std::vector<glm::mat4x3>& value, bool updateGpuData/* = true*/)
    {
        // mat4x3类型可以简写
        setBufferData(varname, &value, sizeof(glm::mat4x3)*value.size());
        
        if (updateGpuData)
        {
            updateGPUResource();
        }
    }
    
    void UniformBuffer::setData(const string& varname, std::vector<glm::mat4>& value, bool updateGpuData/* = true*/)
    {
        // mat4类型可以简写
        setBufferData(varname, &value, sizeof(glm::mat4)*value.size());
        
        if (updateGpuData)
        {
            updateGPUResource();
        }
    }

	void UniformBuffer::createGPUResource()
	{
         auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateUniformBufferCommand>(GPUOperateCommandType::GOCT_UniformBuffer);
         cmd->setUniformBuffer(this);
         cmd->ready(GPUOperateType::GOT_Create);
         GPUOperateSystem::getInstance()->addCommand(cmd);
	}
	
	void UniformBuffer::updateGPUResource()
	{
		 auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateUniformBufferCommand>(GPUOperateCommandType::GOCT_UniformBuffer);
		 cmd->setUniformBuffer(this);
		 cmd->setData(m_uSize, m_vVariableNames, m_mVariableDatas);
		 cmd->ready(GPUOperateType::GOT_Update);
		 GPUOperateSystem::getInstance()->addCommand(cmd);
	}

	void UniformBuffer::deleteGPUResource()
	{
        BROWSER_ASSERT(m_eResouceState==GRS_Loaded, "UniformBuffer state must be GRS_Loaded, then it can be destroyed on gpu");
        
         auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateUniformBufferCommand>(GPUOperateCommandType::GOCT_UniformBuffer);
		 cmd->setUniformBuffer(this);
         cmd->ready(GPUOperateType::GOT_Delete);
         GPUOperateSystem::getInstance()->addCommand(cmd);
	}
    

}
