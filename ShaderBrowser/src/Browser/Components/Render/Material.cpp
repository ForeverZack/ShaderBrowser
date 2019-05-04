#include "Material.h"
#include "Common/Tools/Utils.h"
#include "Common/System/Cache/GLProgramCache.h"
#include "GL/Texture2D.h"

namespace browser
{
	// 默认材质名称
	const char* Material::DEFAULT_MATERIAL_NAME = "ShaderBrowser_DefaultMaterial";

	Material* Material::createMaterial(const std::string& materialName /*= DEFAULT_MATERIAL_NAME*/)
	{
		Material* material = new Material(materialName);
        
		material->init();
        material->autorelease();
        
		return material;
	}
    
    Material* Material::createMaterial(const std::string& programName, const std::string& materialName)
    {
        GLProgram* program = GLProgramCache::getInstance()->getGLProgramCopy(programName);
        Pass* pass = Pass::createPass(program);
        
        Material* material = Material::createMaterial(materialName);
        material->addPass(pass);
        
        return material;
    }

	Material::Material(const std::string& materialName)
		: m_sMaterialName(materialName)
        , m_uMaterialId(0)
        , m_eQueue(RenderQueue::Geometry)
        , m_bGpuInstance(false)
        , m_uSharedId(0)
        , m_bDefaultMaterialFlag(false)
        , m_bTransparentFlag(false)
        , m_bInitColorProperties(false)
        , m_oCurCamera(nullptr)
	{
        m_vPass.clear();
	}

	Material::~Material()
	{
        for(auto itor=m_vPass.begin(); itor!=m_vPass.end(); ++itor)
        {
            (*itor)->release();
        }
	}

	void Material::init()
	{
        m_mUniforms.clear();
        m_vPass.clear();
	}

    void Material::addPass(Pass* pass)
    {
        pass->retain();
        m_vPass.push_back(pass);
        
        // 设置pass的uniform数据来源
        pass->setUniformsFromMaterial(&m_mUniforms);
    }
    
	void Material::useMaterial(Mesh* mesh, Transform* transform, Camera* camera, int index/* = 0*/)
	{
        common::BROWSER_ASSERT(m_vPass.size()>index && m_vPass[index], "cannot found pass in function Material::useMaterial");
        
        // 应用网格的纹理
        const std::unordered_map<std::string, TextureData>& textureInfos = mesh->getTextures();
        Texture2D* texture;
        for (auto itor = textureInfos.cbegin(); itor!=textureInfos.cend(); ++itor)
        {
            texture = itor->second.texture;
            setUniformTex2D(itor->second.uniformName.c_str(), texture->getTextureId());
        }
        
        // 设置uniform（uniform需要每帧更新！！！！）
        // 设置模型材质属性到pass （TODO: pass理论上可以修改它们的值，应该做判断，这里先覆盖）
        if (!m_bInitColorProperties)
        {
            m_bInitColorProperties = true;
            
            const std::unordered_map<std::string, glm::vec4>& colorProperties = mesh->getColorProperties();
            for (auto itor = colorProperties.begin(); itor != colorProperties.end(); ++itor)
            {
                setUniformV4f(itor->first, itor->second);
            }
        }
        
        
        // 设置model矩阵
        if (transform->getCurFrameDirty())
        {
            setUniformMat4(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_MODEL_MATRIX], transform->getModelMatrix());
        }
        // 设置view矩阵,projection矩阵
        if (camera!=m_oCurCamera || camera->getTransDirty())
        {
            m_oCurCamera = camera;
            
            setUniformMat4(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_VIEW_MATRIX], camera->getViewMatrix());
            setUniformMat4(GLProgram::SHADER_UNIFORMS_ARRAY[GLProgram::UNIFORM_CGL_PROJECTION_MATRIX], camera->getProjectionMatrix());
        }
        
        // 使用glProgram
        m_vPass[index]->usePass();
	}
    
    void Material::setUniformInt(const std::string& uniformName, int value)
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
    void Material::setUniformFloat(const std::string& uniformName, float value)
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
    void Material::setUniformMat3(const std::string& uniformName, const glm::mat3& value)
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
    void Material::setUniformMat3x4(const std::string& uniformName, const glm::mat3x4& value)
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
    void Material::setUniformMat4(const std::string& uniformName, const glm::mat4& value)
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
    
    void Material::setUniformMat4x3(const std::string& uniformName, const glm::mat4x3& value)
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
    
    void Material::setUniformFloatV(const std::string& uniformName, int size, const float* value)
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
    
    void Material::setUniformV2f(const std::string& uniformName, const glm::vec2& value)
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
    
    void Material::setUniformV3f(const std::string& uniformName, const glm::vec3& value)
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
    
    void Material::setUniformV4f(const std::string& uniformName, const glm::vec4& value)
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
    
    void Material::setUniformTex2D(const std::string& uniformName, GLuint textureId)
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

    

}
