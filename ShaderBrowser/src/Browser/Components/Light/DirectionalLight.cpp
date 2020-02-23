#include "DirectionalLight.h"

using namespace common;

namespace browser
{
	DirectionalLight::DirectionalLight()
        : m_oLightDirection(GLM_AXIS_X)
    {
        m_sComponentName = "Directional Light";
        // 设置光源类型
        m_eType = LightType::Directional;
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
    
    void DirectionalLight::updateMaterialLight(const std::unordered_map<std::string, UniformValue>& uniforms, unsigned int index/* = 0*/)
    {
        if (BROWSER_GET_BIT(m_uPropertiesDirty, LightPropertyType::LPT_Color))
        {
//            uniforms[]
        }
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
