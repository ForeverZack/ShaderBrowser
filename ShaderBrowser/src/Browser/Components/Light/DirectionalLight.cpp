#include "DirectionalLight.h"
#include "GL/GPUResource/Shader/GLProgram.h"

using namespace common;

namespace browser
{
	DirectionalLight::DirectionalLight()
        : m_oLightDirection(GLM_AXIS_X)
    {
        m_sComponentName = "Directional Light";
        // 设置光源类型
        m_eType = LightType::Directional;

		// 设置脏标记
		m_uPropertiesDirty = LPT_Color | LPT_Intensity | LPT_LightDirection;
		dispatchEventToSystem(SystemType::Light, ComponentEvent::Light_UpdateLight, new UpdateLightMessage(this));
	}

	DirectionalLight::~DirectionalLight()
	{
		BROWSER_LOG("~DirectionalLight");
	}

    void DirectionalLight::onInspectorGUI(InspectorPanel* inspector)
    {
        //
        inspector->addPropertyText("Intensity: " + std::to_string(m_fIntensity));
    }
    
    void DirectionalLight::updateLight()
    {
        
    }
    
    void DirectionalLight::updateAllMaterialsLight(unsigned int index/* = 0*/)
    {
        // 颜色
        if (BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_Color))
        {
            std::string uniformName(GLProgram::SHADER_UNIFORM_NAME_MAX_LENGTH, '\0');
            sprintf(&uniformName[0], SHADER_UNIFORM_DIRECTIONAL_COLOR, index);
			MaterialCache::getInstance()->operateAllMaterials([&](Material* material)  -> void
			{
				material->setUniformV4f(uniformName, m_oColor);
			});
        }
        // 强度
        if (BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_Intensity))
        {
            std::string uniformName(GLProgram::SHADER_UNIFORM_NAME_MAX_LENGTH, '\0');
            sprintf(&uniformName[0], SHADER_UNIFORM_DIRECTIONAL_INTENSITY, index);
			MaterialCache::getInstance()->operateAllMaterials([&](Material* material)  -> void
			{
				material->setUniformFloat(uniformName, m_fIntensity);
			});
        }
        // 光照方向
        if (BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_LightDirection))
        {
            std::string uniformName(GLProgram::SHADER_UNIFORM_NAME_MAX_LENGTH, '\0');
            sprintf(&uniformName[0], SHADER_UNIFORM_DIRECTIONAL_DIRECTION, index);
			MaterialCache::getInstance()->operateAllMaterials([&](Material* material)  -> void
			{
				material->setUniformV3f(uniformName, m_oLightDirection);
			});
        }

		// 重置脏标记
		resetLightDirty();
    }
    
    bool DirectionalLight::isLightDirty()
    {
        if (BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_Color)
            || BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_Intensity)
            || BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_LightDirection))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    
    bool DirectionalLight::isLightSystemDirty()
    {
        return BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_Intensity);
    }
    
    void DirectionalLight::setLightDirection(const glm::vec3& dir)
    {
        m_oGlobalPosition = dir;
        setDirty(LightPropertyType::LPT_LightDirection);
    }
    
    void DirectionalLight::setLightDirection(float x, float y, float z)
    {
        m_oGlobalPosition.x = x;
        m_oGlobalPosition.y = y;
        m_oGlobalPosition.z = z;
        setDirty(LightPropertyType::LPT_LightDirection);
    }
    
}
