#include "Material.h"
#include "Common/Tools/Utils.h"
#include "Common/System/Cache/GLProgramCache.h"
#include "Common/System/Cache/MaterialManager.h"
#include "Common/System/Cache/MaterialCache.h"
#include "Browser/System/LightSystem.h"

namespace customGL
{
	// 默认材质名称
	const char* Material::DEFAULT_MATERIAL_NAME = "ShaderBrowser_DefaultMaterial";

    Material* Material::create(const MaterialParameters* parameters)
    {
		// create material
		Material* material = new Material(parameters->name);
        
		// render queue
		material->setRenderQueue(parameters->renderQueue);

		// depth
		material->setEnableZTest(parameters->ztest.enableZTest);
		material->setZTestFunc(parameters->ztest.ZTestFunc);
		material->setZWrite(parameters->ztest.ZWrite);
		// cull
		material->setEnableCull(parameters->cull.enableCull);
		material->setCullFace(parameters->cull.cullFace);
		material->setFrontFace(parameters->cull.frontFace);
		// stencil
		material->setEnableStencilTest(parameters->stencil.enableStencilTest);
		material->setStencilFuncParameter(parameters->stencil.stencilFuncParam->func, parameters->stencil.stencilFuncParam->ref, parameters->stencil.stencilFuncParam->mask);
		material->setStencilOpParameter(parameters->stencil.stencilOpParam->sfail, parameters->stencil.stencilOpParam->dpfail, parameters->stencil.stencilOpParam->dppass);
		// blend
		material->setEnableBlend(parameters->blend.enableBlend);
		material->setBlendFuncParameter(parameters->blend.blendFuncParam->sfactor, parameters->blend.blendFuncParam->dfactor, parameters->blend.blendFuncParam->safactor, parameters->blend.blendFuncParam->dafactor);
		material->setBlendEquation(parameters->blend.blendEquation);
		
        // pass
        GLProgram* program = nullptr;
        Pass* pass = nullptr;
        for (auto itor=parameters->passes.begin(); itor!=parameters->passes.end(); ++itor)
        {
            const MaterialPassParamter& passParam = *itor;
            if (!GLProgramCache::getInstance()->getGLProgram(passParam.name))
            {
				program = GLProgramCache::getInstance()->addGLProgramBySource(passParam.name, passParam.vert_program, passParam.frag_program);
				program->setProgramTags(passParam.tags);
            }
			program = GLProgramCache::getInstance()->getGLProgramCopy(passParam.name);
			pass = Pass::createPass(program);
			// depth
			pass->setEnableZTest(passParam.ztest.enableZTest);
			pass->setZTestFunc(passParam.ztest.ZTestFunc);
			pass->setZWrite(passParam.ztest.ZWrite);
			// cull
			pass->setEnableCull(passParam.cull.enableCull);
			pass->setCullFace(passParam.cull.cullFace);
			pass->setFrontFace(passParam.cull.frontFace);
			// stencil
			pass->setEnableStencilTest(passParam.stencil.enableStencilTest);
			pass->setStencilFuncParameter(passParam.stencil.stencilFuncParam->func, passParam.stencil.stencilFuncParam->ref, passParam.stencil.stencilFuncParam->mask);
			pass->setStencilOpParameter(passParam.stencil.stencilOpParam->sfail, passParam.stencil.stencilOpParam->dpfail, passParam.stencil.stencilOpParam->dppass);
			// blend
			pass->setEnableBlend(passParam.blend.enableBlend);
			pass->setBlendFuncParameter(passParam.blend.blendFuncParam->sfactor, passParam.blend.blendFuncParam->dfactor, passParam.blend.blendFuncParam->safactor, passParam.blend.blendFuncParam->dafactor);
			pass->setBlendEquation(passParam.blend.blendEquation);

            material->addPass(pass);

			// pre-pass  (如果存在多个pass支持[PreparePass]标签，则只使用第一个支持的Pass)
			if (!material->getPrePass() && program->isSupportPrePass())
			{
				program = GLProgramCache::getInstance()->getGLProgramCopy(passParam.name, 1 << GLProgram::GLProgramTag::GLProgramTag_PrePass);
				pass = Pass::createPass(program);
				material->setPrePass(pass);
			}
			// shadow caster pass (如果存在多个pass支持[ShadowCaster]标签，则只使用第一个支持的Pass)
			if (!material->getShadowCasterPass() && program->isSupportShadowCaster())
			{
				program = GLProgramCache::getInstance()->getGLProgramCopy(passParam.name, 1 << GLProgram::GLProgramTag::GLProgramTag_ShadowCaster);
				pass = Pass::createPass(program);
				material->setShadowCasterPass(pass);
			}
        }
        
        // uniforms (注意！！！这里Material的材质必须提前加载完成！！)
        MaterialParser::setupMaterialUniforms(parameters->uniforms, material);
        
        return material;
    }
    
    Material* Material::createMaterialByProgramName(const std::string& materialName, const std::string& programName)
    {
		// create material
		Material* material = new Material(materialName);

		GLProgram* program = nullptr;
		Pass* pass = nullptr;

		// 因为只有发生改变的UniformValue会在GPU中重新设置，所以每个Material对应的GLProgram都应该是独立的，可合并的应该是相同的material
        program = GLProgramCache::getInstance()->getGLProgramCopy(programName);
        pass = Pass::createPass(program);
        material->addPass(pass);

		// pre-pass  (如果存在多个pass支持[PreparePass]标签，则只使用第一个支持的Pass)
		if (program->isSupportPrePass())
		{
			program = GLProgramCache::getInstance()->getGLProgramCopy(programName, 1 << GLProgram::GLProgramTag::GLProgramTag_PrePass);
			pass = Pass::createPass(program);
			material->setPrePass(pass);
		}
		// shadow caster pass (如果存在多个pass支持[ShadowCaster]标签，则只使用第一个支持的Pass)
		if (program->isSupportShadowCaster())
		{
			program = GLProgramCache::getInstance()->getGLProgramCopy(programName, 1 << GLProgram::GLProgramTag::GLProgramTag_ShadowCaster);
			pass = Pass::createPass(program);
			material->setShadowCasterPass(pass);
		}
        
        return material;
    }

	Material::Material(const std::string& materialName)
		: m_sMaterialName(materialName)
		, m_sFilePath("")
        , m_uMaterialId(0)
        , m_eQueue(RenderQueue::Opaque)
        , m_bGpuInstance(false)
        , m_uSharedId(0)
        , m_bDefaultMaterialFlag(false)
        , m_bTransparentFlag(false)
        , m_oCurCamera(nullptr)
		, m_bDirty(true)
		, m_pPrePass(nullptr)
		, m_pShadowCasterPass(nullptr)
		, m_uViewMatrixDirtyTag(0)
		, m_uProjectionMatrixDirtyTag(0)
		// depth test
		, m_bEnableZTest(true)
		, m_eZTestFunc(GL_LESS)
		, m_bZWrite(true)
		// cull face
		, m_bEnableCull(true)
		, m_eCullFace(GL_BACK)
		, m_eFrontFace(GL_CCW)
		// stencil test
		, m_bEnableStencilTest(false)
		, m_pStencilFuncParam(nullptr)
		, m_pStencilOpParam(nullptr)
		// blend
		, m_bEnableBlend(false)
		, m_pBlendFuncParam(nullptr)
		, m_eBlendEquation(GL_FUNC_ADD)
	{
		this->autorelease();

        m_vPass.clear();
	
		// 新的材质加入MaterialCache
		MaterialManager::getInstance()->addMaterial(this);
		// 加入待处理的光照材质
		LightSystem::getInstance()->addPrepareLightMaterial(this);
	}

	Material::~Material()
	{
		// passes
        for(auto itor=m_vPass.begin(); itor!=m_vPass.end(); ++itor)
        {
            (*itor)->release();
        }
		// pre-pass
		BROWSER_SAFE_RELEASE_REFERENCE(m_pPrePass);
		// shadow caster pass
		BROWSER_SAFE_RELEASE_REFERENCE(m_pShadowCasterPass);

		// 从LightSystem中移除
		LightSystem::getInstance()->removeLightMaterial(this);
		// 从MaterialCache移除
		MaterialCache::getInstance()->removeMaterial(this);
		// 从MaterialManager中移除
		MaterialManager::getInstance()->removeMaterial(this);
	}

    void Material::addPass(Pass* pass)
    {
        pass->retain();
        m_vPass.push_back(pass);
    }
    
	void Material::useMaterial(bool transformDirty, const glm::mat4& modelMatrix, bool cameraDirty, const glm::vec3& cameraGlobalPos, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, std::unordered_map<std::string, UniformValue>& uniforms, int index/* = 0*/)
	{
        common::BROWSER_ASSERT(m_vPass.size()>index && m_vPass[index], "cannot found pass in function Material::useMaterial");
        
        // 使用glProgram	
        //m_vPass[index]->usePass(transformDirty, modelMatrix, cameraDirty, cameraGlobalPos, viewMatrix, projectionMatrix, uniforms);
		m_vPass[index]->usePass(transformDirty, modelMatrix, cameraDirty, cameraGlobalPos, viewMatrix, projectionMatrix, uniforms
												, m_bEnableZTest, m_eZTestFunc, m_bZWrite
												, m_bEnableCull, m_eCullFace, m_eFrontFace
												, m_bEnableStencilTest, m_pStencilFuncParam, m_pStencilOpParam
												, m_bEnableBlend, m_pBlendFuncParam, m_eBlendEquation);
	}

	void Material::useMaterial_PrePass(bool transformDirty, const glm::mat4& modelMatrix, bool cameraDirty, const glm::vec3& cameraGlobalPos, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, std::unordered_map<std::string, UniformValue>& uniforms)
	{
		common::BROWSER_ASSERT(m_pPrePass, "cannot found pass in function Material::useMaterial_PrePass");

		// 使用glProgram	
		m_pPrePass->usePass(transformDirty, modelMatrix, cameraDirty, cameraGlobalPos, viewMatrix, projectionMatrix, uniforms
			, m_bEnableZTest, m_eZTestFunc, m_bZWrite
			, m_bEnableCull, m_eCullFace, m_eFrontFace
			, m_bEnableStencilTest, m_pStencilFuncParam, m_pStencilOpParam
			, m_bEnableBlend, m_pBlendFuncParam, m_eBlendEquation);
	}

	void Material::useMaterial_ShadowCasterPass(bool transformDirty, const glm::mat4& modelMatrix, bool cameraDirty, const glm::vec3& cameraGlobalPos, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, std::unordered_map<std::string, UniformValue>& uniforms)
	{
		common::BROWSER_ASSERT(m_pShadowCasterPass, "cannot found pass in function Material::useMaterial_ShadowCasterPass");

		// 使用glProgram	
		m_pShadowCasterPass->usePass(transformDirty, modelMatrix, cameraDirty, cameraGlobalPos, viewMatrix, projectionMatrix, uniforms
			, m_bEnableZTest, m_eZTestFunc, m_bZWrite
			, m_bEnableCull, m_eCullFace, m_eFrontFace
			, m_bEnableStencilTest, m_pStencilFuncParam, m_pStencilOpParam
			, m_bEnableBlend, m_pBlendFuncParam, m_eBlendEquation);
	}

	void Material::setEnableStencilTest(bool enable)
	{
		m_bEnableStencilTest = enable;
		if (enable)
		{
			if (!m_pStencilFuncParam)
			{
				m_pStencilFuncParam = make_shared<StencilFuncParameter>(0, 0xFF);
			}
			if (!m_pStencilOpParam)
			{
				m_pStencilOpParam = make_shared<StencilOpParameter>(GL_KEEP, GL_KEEP, GL_REPLACE);
			}
		}
	}

	void Material::setStencilFuncParameter(GLint ref, GLuint mask)
	{
		if (!m_pStencilFuncParam)
		{
			m_pStencilFuncParam = make_shared<StencilFuncParameter>(ref, mask);
		}
		else
		{
			m_pStencilFuncParam->func = GL_EQUAL;
			m_pStencilFuncParam->ref = ref;
			m_pStencilFuncParam->mask = mask;
		}
	}

	void Material::setStencilFuncParameter(GLenum func, GLint ref, GLuint mask)
	{
		if (!m_pStencilFuncParam)
		{
			m_pStencilFuncParam = make_shared<StencilFuncParameter>(func, ref, mask);
		}
		else
		{
			m_pStencilFuncParam->func = func;
			m_pStencilFuncParam->ref = ref;
			m_pStencilFuncParam->mask = mask;
		}
	}

	void Material::setStencilOpParameter(GLenum sfail, GLenum dpfail, GLenum dppass)
	{
		if (!m_pStencilOpParam)
		{
			m_pStencilOpParam = make_shared<StencilOpParameter>(sfail, dpfail, dppass);
		}
		else
		{
			m_pStencilOpParam->sfail = sfail;
			m_pStencilOpParam->dpfail = dpfail;
			m_pStencilOpParam->dppass = dppass;
		}
	}

	void Material::setEnableBlend(bool enable)
	{
		m_bEnableBlend = enable;
		if (enable)
		{
			if (!m_pBlendFuncParam)
			{
				m_pBlendFuncParam = std::make_shared<BlendFuncParameter>(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
		}
	}

	void Material::setBlendFuncParameter(GLenum sfactor, GLenum dfactor)
	{
		if (!m_pBlendFuncParam)
		{
			m_pBlendFuncParam = std::make_shared<BlendFuncParameter>(sfactor, dfactor);
		}
		else
		{
			m_pBlendFuncParam->sfactor = sfactor;
			m_pBlendFuncParam->dfactor = dfactor;
			m_pBlendFuncParam->safactor = sfactor;
			m_pBlendFuncParam->dafactor = dfactor;
		}
	}

	void Material::setBlendFuncParameter(GLenum sfactor, GLenum dfactor, GLenum safactor, GLenum dafactor)
	{
		if (!m_pBlendFuncParam)
		{
			m_pBlendFuncParam = std::make_shared<BlendFuncParameter>(sfactor, dfactor, safactor, dafactor);
		}
		else
		{
			m_pBlendFuncParam->sfactor = sfactor;
			m_pBlendFuncParam->dfactor = dfactor;
			m_pBlendFuncParam->safactor = safactor;
			m_pBlendFuncParam->dafactor = dafactor;
		}
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
    
    void Material::setUniformTex2D(const std::string& uniformName, RenderTexture* texture)
    {
        Utils::setUniformTex2D(m_mUniforms, uniformName, texture);
    }
    
    void Material::setUniformSamplerBuffer(const std::string& uniformName, TextureBuffer* textureBuffer)
    {
        Utils::setUniformSamplerBuffer(m_mUniforms, uniformName, textureBuffer);
    }

	void Material::setUniformUniformBuffer(const std::string& uniformName, UniformBuffer* uniformBuffer)
	{
		Utils::setUniformUniformBuffer(m_mUniforms, uniformName, uniformBuffer);
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

	void Material::setPrePass(Pass* pass)
	{
		BROWSER_SAFE_RELEASE_REFERENCE(m_pPrePass);
		m_pPrePass = pass;
		pass->retain();
	}

	void Material::setShadowCasterPass(Pass* pass)
	{
		BROWSER_SAFE_RELEASE_REFERENCE(m_pShadowCasterPass);
		m_pShadowCasterPass = pass;
		pass->retain();
	}

	bool Material::isSupportPrePass()
	{
		return m_pPrePass != nullptr;
	}

	bool Material::isSupportShadowCaster()
	{
		return m_pShadowCasterPass != nullptr;
	}
    
    

}
