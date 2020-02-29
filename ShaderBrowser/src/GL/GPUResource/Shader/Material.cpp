#include "Material.h"
#include "Common/Tools/Utils.h"
#include "Common/System/Cache/GLProgramCache.h"
#include "Common/System/Cache/MaterialCache.h"
#include "Browser/System/LightSystem.h"

namespace customGL
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
		// 因为只有发生改变的UniformValue会在GPU中重新设置，所以每个Material对应的GLProgram都应该是独立的，可合并的应该是相同的material
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
        , m_oCurCamera(nullptr)
	{
        m_vPass.clear();
	
		// 新的材质加入MaterialCache
		MaterialCache::getInstance()->addMaterial(this);
		// 加入待处理的光照材质
		LightSystem::getInstance()->addPrepareLightMaterial(this);
	}

	Material::~Material()
	{
		// 从MaterialCache中移除材质
		MaterialCache::getInstance()->removeMaterial(this);

        for(auto itor=m_vPass.begin(); itor!=m_vPass.end(); ++itor)
        {
            (*itor)->release();
        }

		// 从待处理的光照材质移除
		LightSystem::getInstance()->removePrepareLightMaterial(this);
		// 从MaterialCache移除
		MaterialCache::getInstance()->removeMaterial(this);
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
    }
    
	void Material::useMaterial(bool transformDirty, const glm::mat4& modelMatrix, bool cameraDirty, const glm::vec3& cameraGlobalPos, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, std::unordered_map<std::string, UniformValue>& uniforms, int index/* = 0*/)
	{
        common::BROWSER_ASSERT(m_vPass.size()>index && m_vPass[index], "cannot found pass in function Material::useMaterial");
        
        // 使用glProgram		TODO: usePass会改变UniformValue的dirty状态，如果是多个pass会出问题的！！
        m_vPass[index]->usePass(transformDirty, modelMatrix, cameraDirty, cameraGlobalPos, viewMatrix, projectionMatrix, uniforms);
	}

	bool Material::isGPUResourceLoaded()
	{
		for (int i = 0; i < m_vPass.size(); ++i)
		{
			if (!m_vPass[i]->isGPUResourceLoaded())
			{
				return false;
			}
		}
		return true;
	}
    
    void Material::setUniformInt(const std::string& uniformName, int value)
    {
        Utils::setUniformInt(m_mUniforms, uniformName, value);
    }
    void Material::setUniformFloat(const std::string& uniformName, float value)
    {
        Utils::setUniformFloat(m_mUniforms, uniformName, value);
    }
    void Material::setUniformMat3(const std::string& uniformName, const glm::mat3& value)
    {
        Utils::setUniformMat3(m_mUniforms, uniformName, value);
    }
    void Material::setUniformMat3x4(const std::string& uniformName, const glm::mat3x4& value)
    {
        Utils::setUniformMat3x4(m_mUniforms, uniformName, value);
    }
    void Material::setUniformMat4(const std::string& uniformName, const glm::mat4& value)
    {
        Utils::setUniformMat4(m_mUniforms, uniformName, value);
    }
    
    void Material::setUniformMat4x3(const std::string& uniformName, const glm::mat4x3& value)
    {
        Utils::setUniformMat4x3(m_mUniforms, uniformName, value);
    }
    
    void Material::setUniformFloatV(const std::string& uniformName, int size, const float* value)
    {
        Utils::setUniformFloatV(m_mUniforms, uniformName, size, value);
    }
    
    void Material::setUniformV2f(const std::string& uniformName, const glm::vec2& value)
    {
        Utils::setUniformV2f(m_mUniforms, uniformName, value);
    }
    
    void Material::setUniformV3f(const std::string& uniformName, const glm::vec3& value)
    {
        Utils::setUniformV3f(m_mUniforms, uniformName, value);
    }
    
    void Material::setUniformV4f(const std::string& uniformName, const glm::vec4& value)
    {
        Utils::setUniformV4f(m_mUniforms, uniformName, value);
    }
    
    void Material::setUniformTex2D(const std::string& uniformName, Texture2D* texture)
    {
        Utils::setUniformTex2D(m_mUniforms, uniformName, texture);
    }
    
    void Material::setUniformSamplerBuffer(const std::string& uniformName, TextureBuffer* textureBuffer)
    {
        Utils::setUniformSamplerBuffer(m_mUniforms, uniformName, textureBuffer);
    }

    void Material::setUniformIntV(const std::string& uniformName, int count, const int* value)
    {
        Utils::setUniformIntV(m_mUniforms, uniformName, count, value);
    }
    
    void Material::setUniformIVec2(const std::string& uniformName, const glm::ivec2& value)
    {
        Utils::setUniformIVec2(m_mUniforms, uniformName, value);
    }
    
    void Material::setUniformVec2V(const std::string& uniformName, int count, const float* value)
    {
        Utils::setUniformVec2V(m_mUniforms, uniformName, count, value);
    }
    
    void Material::setUniformIVec2V(const std::string& uniformName, int count, const int* value)
    {
        Utils::setUniformIVec2V(m_mUniforms, uniformName, count, value);
    }
    
    void Material::setUniformIVec3(const std::string& uniformName, const glm::ivec3& value)
    {
        Utils::setUniformIVec3(m_mUniforms, uniformName, value);
    }
    
    void Material::setUniformVec3V(const std::string& uniformName, int count, const float* value)
    {
        Utils::setUniformVec3V(m_mUniforms, uniformName, count, value);
    }
    
    void Material::setUniformIVec3V(const std::string& uniformName, int count, const int* value)
    {
        Utils::setUniformIVec3V(m_mUniforms, uniformName, count, value);
    }
    
    void Material::setUniformIVec4(const std::string& uniformName, const glm::ivec4& value)
    {
        Utils::setUniformIVec4(m_mUniforms, uniformName, value);
    }
    
    void Material::setUniformVec4V(const std::string& uniformName, int count, const float* value)
    {
        Utils::setUniformVec4V(m_mUniforms, uniformName, count, value);
    }
    
    void Material::setUniformIVec4V(const std::string& uniformName, int count, const int* value)
    {
        Utils::setUniformIVec4V(m_mUniforms, uniformName, count, value);
    }
    
    void Material::setUniformMat4V(const std::string& uniformName, int count, const float* value)
    {
        Utils::setUniformMat4V(m_mUniforms, uniformName, count, value);
    }
    
    void Material::setUniformMat4x3V(const std::string& uniformName, int count, const float* value)
    {
        Utils::setUniformMat4x3V(m_mUniforms, uniformName, count, value);
    }
    
    void Material::setUniformMat3V(const std::string& uniformName, int count, const float* value)
    {
        Utils::setUniformMat3V(m_mUniforms, uniformName, count, value);
    }
    
    void Material::setUniformMat3x4V(const std::string& uniformName, int count, const float* value)
    {
        Utils::setUniformMat3x4V(m_mUniforms, uniformName, count, value);
    }
    
    void Material::resetUniformsDirty()
    {
        for (auto itor=m_mUniforms.begin(); itor!=m_mUniforms.end(); ++itor)
        {
            itor->second.resetDirty();
        }
    }
    
    

}
