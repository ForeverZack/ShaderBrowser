#include "Pass.h"

namespace browser
{
	Pass* Pass::createPass(GLProgram* program)
	{
		Pass* pass = new Pass();
		pass->init(program);

		return pass;
	}

	Pass::Pass()
		: m_oGLProgram(nullptr)
	{
        m_mUniforms.clear();
	}

	Pass::~Pass()
	{

	}

	void Pass::init(GLProgram* program)
	{
		m_oGLProgram = program;
	}
    
    void Pass::setUniformInt(const char* uniformName, int value)
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
    void Pass::setUniformFloat(const char* uniformName, float value)
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
    void Pass::setUniformMat3(const char* uniformName, glm::mat3 value)
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
    void Pass::setUniformMat4(const char* uniformName, glm::mat4 value)
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
    
    void Pass::setUniformFloatV(const char* uniformName, int size, const float* value)
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
    
    void Pass::setUniformV2f(const char* uniformName, glm::vec2 value)
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
    
    void Pass::setUniformV3f(const char* uniformName, glm::vec3 value)
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

	void Pass::setUniformV4f(const char* uniformName, glm::vec4 value)
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
    
    void Pass::setUniformTex2D(const char* uniformName, GLuint textureId)
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

	void Pass::usePass()
	{
		// 注意在更新uniform变量的值之前，要先使用着色器glUseProgram
		m_oGLProgram->useProgram();

		// 设置uniform（uniform需要每帧更新！！！！）
		for (auto itor = m_mUniforms.begin(); itor != m_mUniforms.end(); ++itor)
		{
            itor->second.updateGLProgramUniformValue(m_oGLProgram, itor->first);
		}
	}


}
