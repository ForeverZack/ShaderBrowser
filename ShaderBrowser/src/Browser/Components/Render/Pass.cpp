#include "Pass.h"
#include "../../../GL/Texture2D.h"

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
    
    void Pass::setUniformInt(const std::string& uniformName, int value)
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
    void Pass::setUniformFloat(const std::string& uniformName, float value)
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
    void Pass::setUniformMat3(const std::string& uniformName, glm::mat3 value)
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
    void Pass::setUniformMat4(const std::string& uniformName, glm::mat4 value)
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
    
    void Pass::setUniformFloatV(const std::string& uniformName, int size, const float* value)
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
    
    void Pass::setUniformV2f(const std::string& uniformName, glm::vec2 value)
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
    
    void Pass::setUniformV3f(const std::string& uniformName, glm::vec3 value)
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

	void Pass::setUniformV4f(const std::string& uniformName, glm::vec4 value)
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
    
    void Pass::setUniformTex2D(const std::string& uniformName, GLuint textureId)
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

	void Pass::usePass(Mesh* mesh)
	{
        // 应用网格过滤器中的纹理
        const std::vector<TextureData>& textureInfos = mesh->getTextures();
        Texture2D* texture;
        for (auto itor = textureInfos.cbegin(); itor!=textureInfos.cend(); ++itor)
        {
            texture = itor->texture;
            setUniformTex2D(itor->uniformName.c_str(), texture->getTextureId());
        }
        
		// 注意在更新uniform变量的值之前，要先使用着色器glUseProgram
		m_oGLProgram->useProgram();

		// 设置uniform（uniform需要每帧更新！！！！）
		for (auto itor = m_mUniforms.begin(); itor != m_mUniforms.end(); ++itor)
		{
            itor->second.updateGLProgramUniformValue(m_oGLProgram, itor->first);
		}
	}


}
