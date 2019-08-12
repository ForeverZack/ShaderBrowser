#include "BaseComputeProgram.h"
#include <glad/glad.h>
#include "Browser/System/RenderSystem.h"
#include <chrono>

using namespace customGL;

namespace browser
{
	BaseComputeProgram::BaseComputeProgram()
        : m_oComputeProgram(nullptr)
		, m_uTexNum(0)
	{
        // clear
		m_vTextureBuffers.clear();
        m_mUniforms.clear();
		m_mBufferIds.clear();
	}

	BaseComputeProgram::~BaseComputeProgram()
	{
		// 释放vbo和tex
		glDeleteBuffers(m_vTextureBuffers.size(), m_vVBOs);
		if (m_uTexNum > 0)
		{
			glDeleteTextures(m_uTexNum, m_vTexs);
		}

        // 释放Compute Program对象
		m_oComputeProgram->release();

		// 释放buffer声明
		for (auto itor = m_vTextureBuffers.begin(); itor != m_vTextureBuffers.end(); ++itor)
		{
			BROWSER_SAFE_RELEASE_POINTER(*itor);
		}
		m_vTextureBuffers.clear();
	}
    
	bool BaseComputeProgram::initComputeProgram(const char* src, int local_size_x/* = 1*/, int local_size_y/* = 1*/, int local_size_z/* = 1*/, int num_groups_x/* = 1*/, int num_groups_y/* = 1*/, int num_groups_z/* = 1*/)
	{
		m_oComputeProgram = GLComputeProgram::create(src, local_size_x, local_size_y, local_size_z, num_groups_x, num_groups_y, num_groups_z);
		m_oComputeProgram->retain();

		return true;
	}
    
    void BaseComputeProgram::setupVBOs()
    {
        // vbos
        int idx = 0;
		GLuint vbo;
        {
            int texIdx = 0;
            ComputeBufferDeclaration* declaration;
            glGenBuffers(m_vTextureBuffers.size(), m_vVBOs);
            glGenTextures(m_uTexNum, m_vTexs);
            for (auto itor=m_vTextureBuffers.begin(); itor!= m_vTextureBuffers.end(); ++itor)
            {
                declaration = *itor;
				vbo = m_vVBOs[idx];
				m_mBufferIds[declaration->name] = vbo;
                    
                switch(declaration->type)
                {
                    case BufferType::StorageBuffer:
						{
							// data
							//glBindBuffer(GL_ARRAY_BUFFER, declaration->vbos[i]);
							//glBufferData(GL_ARRAY_BUFFER, declaration->size, nullptr, GL_DYNAMIC_COPY);
						}
                        break;
                            
                    case BufferType::TextureBuffer:
						{
							// tbo
							glBindBuffer(GL_TEXTURE_BUFFER, vbo);
							glBufferData(GL_TEXTURE_BUFFER, declaration->size, nullptr, GL_DYNAMIC_COPY);
							// 将缓存区关联到纹理对象上tbo
							glBindTexture(GL_TEXTURE_BUFFER, m_vTexs[texIdx]);
							glTexBuffer(GL_TEXTURE_BUFFER, declaration->format, vbo);
							// 设置uniform
							setUniformImageBuffer(declaration->name, m_vTexs[texIdx], declaration->access, declaration->format);

							++texIdx;
						}
                        break;
                }
                    
                ++idx;
            }

        }
    }

	void BaseComputeProgram::executeComputeProgram()
	{
		m_oComputeProgram->useProgram();

		// 如果有tbo存在的话，需要重新设置纹理单元
		for (auto itor = m_mUniforms.begin(); itor != m_mUniforms.end(); ++itor)
		{
			const UniformValue& uniform = itor->second;
			if (uniform.getUniformValueType() == UniformValue::UniformValueType::UniformValueType_ImageBuffer)
			{
				setUniformImageBuffer(itor->first, uniform._value.imageBuffer.textureId, uniform._value.imageBuffer.access, uniform._value.imageBuffer.format);
			}
		}
		// 更新属性
		for (auto itor = m_mUniforms.begin(); itor != m_mUniforms.end(); ++itor)
		{
			itor->second.updateGLProgramUniformValue(m_oComputeProgram, itor->first);
		}

		m_oComputeProgram->executeComputeProgram();
	}
	void BaseComputeProgram::addComputeBuffer(const std::string& name, GLuint size, GLenum access/* = GL_READ_ONLY*/, BufferType type/* = BufferType::TextureBuffer*/, GLenum format/* = GL_RGBA32F*/)
	{
		ComputeBufferDeclaration* declaration = Utils::createComputeBufferDeclaration(name, size, access, type, format);
		m_vTextureBuffers.push_back(declaration);
		        
		// 记录tbo数量
		if (type == BufferType::TextureBuffer)
		{
		    ++m_uTexNum;
		}
	}

    void BaseComputeProgram::setUniformInt(const std::string& uniformName, int value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setInt(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setInt(value);
        }
    }
    void BaseComputeProgram::setUniformFloat(const std::string& uniformName, float value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setFloat(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setFloat(value);
        }
    }
    void BaseComputeProgram::setUniformMat3(const std::string& uniformName, const glm::mat3& value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat3(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat3(value);
        }
    }
    void BaseComputeProgram::setUniformMat3x4(const std::string& uniformName, const glm::mat3x4& value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat3x4(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat3x4(value);
        }
    }
    void BaseComputeProgram::setUniformMat4(const std::string& uniformName, const glm::mat4& value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat4(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat4(value);
        }
    }
    
    void BaseComputeProgram::setUniformMat4x3(const std::string& uniformName, const glm::mat4x3& value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat4x3(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat4x3(value);
        }
    }
    
    void BaseComputeProgram::setUniformFloatV(const std::string& uniformName, int size, const float* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setFloatV(size, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setFloatV(size, value);
        }
    }
    
    void BaseComputeProgram::setUniformV2f(const std::string& uniformName, const glm::vec2& value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setVec2(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setVec2(value);
        }
    }
    
    void BaseComputeProgram::setUniformV3f(const std::string& uniformName, const glm::vec3& value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setVec3(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setVec3(value);
        }
    }
    
    void BaseComputeProgram::setUniformV4f(const std::string& uniformName, const glm::vec4& value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setVec4(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setVec4(value);
        }
    }
    
    void BaseComputeProgram::setUniformTex2D(const std::string& uniformName, GLuint textureId)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setTex2D(textureId);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setTex2D(textureId);
        }
    }
    
    void BaseComputeProgram::setUniformSamplerBuffer(const std::string& uniformName, GLuint textureId)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setSamplerBuffer(textureId);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setSamplerBuffer(textureId);
        }
    }
    
	void BaseComputeProgram::setUniformImageBuffer(const std::string& uniformName, GLuint textureId, GLenum access, GLenum format)
	{
		auto itor = m_mUniforms.find(uniformName);
		if (itor == m_mUniforms.end())
		{
			UniformValue uniformValue;
			uniformValue.setImageBuffer(textureId, access, format);
			m_mUniforms.emplace(uniformName, std::move(uniformValue));
		}
		else
		{
			UniformValue& uniformValue = itor->second;
			uniformValue.setImageBuffer(textureId, access, format);
		}
	}

    void BaseComputeProgram::setUniformIntV(const std::string& uniformName, int count, const int* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setIntV(count, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setIntV(count, value);
        }
    }
    
    void BaseComputeProgram::setUniformIVec2(const std::string& uniformName, const glm::ivec2& value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setIVec2(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setIVec2(value);
        }
    }
    
    void BaseComputeProgram::setUniformVec2V(const std::string& uniformName, int count, const float* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setVec2V(count, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setVec2V(count, value);
        }
    }
    
    void BaseComputeProgram::setUniformIVec2V(const std::string& uniformName, int count, const int* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setIVec2V(count, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setIVec2V(count, value);
        }
    }
    
    void BaseComputeProgram::setUniformIVec3(const std::string& uniformName, const glm::ivec3& value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setIVec3(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setIVec3(value);
        }
    }
    
    void BaseComputeProgram::setUniformVec3V(const std::string& uniformName, int count, const float* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setVec3V(count, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setVec3V(count, value);
        }
    }
    
    void BaseComputeProgram::setUniformIVec3V(const std::string& uniformName, int count, const int* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setIVec3V(count, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setIVec3V(count, value);
        }
    }
    
    void BaseComputeProgram::setUniformIVec4(const std::string& uniformName, const glm::ivec4& value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setIVec4(value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setIVec4(value);
        }
    }
    
    void BaseComputeProgram::setUniformVec4V(const std::string& uniformName, int count, const float* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setVec4V(count, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setVec4V(count, value);
        }
    }
    
    void BaseComputeProgram::setUniformIVec4V(const std::string& uniformName, int count, const int* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setIVec4V(count, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setIVec4V(count, value);
        }
    }
    
    void BaseComputeProgram::setUniformMat4V(const std::string& uniformName, int count, const float* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat4V(count, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat4V(count, value);
        }
    }
    
    void BaseComputeProgram::setUniformMat4x3V(const std::string& uniformName, int count, const float* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat4x3V(count, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat4x3V(count, value);
        }
    }
    
    void BaseComputeProgram::setUniformMat3V(const std::string& uniformName, int count, const float* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat3V(count, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat3V(count, value);
        }
    }
    
    void BaseComputeProgram::setUniformMat3x4V(const std::string& uniformName, int count, const float* value)
    {
        auto itor = m_mUniforms.find(uniformName);
        if (itor == m_mUniforms.end())
        {
            UniformValue uniformValue;
            uniformValue.setMat3x4V(count, value);
            m_mUniforms.emplace(uniformName, std::move(uniformValue));
        }
        else
        {
            UniformValue& uniformValue = itor->second;
            uniformValue.setMat3x4V(count, value);
        }
    }
    
    

}
