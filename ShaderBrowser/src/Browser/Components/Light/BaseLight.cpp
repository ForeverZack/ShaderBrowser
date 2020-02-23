#include "BaseLight.h"
#include "Browser/Entitys/BaseEntity.h"

namespace browser
{
    // uniform变量名称
    // 平行光颜色
    const char* BaseLight::SHADER_UNIFORM_DIRECTIONAL_COLOR = "CGL_DIRECTIONAL_LIGHTS[%d].color";
    // 平行光强度
    const char* BaseLight::SHADER_UNIFORM_DIRECTIONAL_INTENSITY = "CGL_DIRECTIONAL_LIGHTS[%d].intensity";;
    // 平行光方向
    const char* BaseLight::SHADER_UNIFORM_DIRECTIONAL_DIRECTION = "CGL_DIRECTIONAL_LIGHTS[%d].direction";;
    
	BaseLight::BaseLight()
        : BaseComponent("Light")
        , m_oColor(GLM_COLOR_WHITE)
        , m_fIntensity(1.0f)
        , m_uPropertiesDirty(0)
    {
		// 组件所属系统
		m_eBelongSystem = SystemType::Light;
	}

	BaseLight::~BaseLight()
	{
		BROWSER_LOG("~BaseLight");
	}
    
    void BaseLight::updateLight()
    {
        Transform* transform = m_oBelongEntity ? m_oBelongEntity->getComponent<Transform>() : nullptr;
        if (transform)
        {
            const glm::vec3 globalPosition = transform->getGlobalPosition();
            if (m_oGlobalPosition != globalPosition)
            {
                setGlobalPosition(globalPosition);
            }
        }
    }
    
    bool BaseLight::isLightDirty()
    {
        return m_uPropertiesDirty > 0;
    }
    
    bool BaseLight::isLightSystemDirty()
    {
        return m_uPropertiesDirty > 0;
    }
    
    void BaseLight::resetLightDirty()
    {
        m_uPropertiesDirty = 0;
    }
    
    void BaseLight::setColor(const glm::vec4& color)
    {
        m_oColor = color;
        setDirty(LightPropertyType::LPT_Color);
    }
    
    void BaseLight::setIntensity(float intensity)
    {
        m_fIntensity = intensity;
        setDirty(LightPropertyType::LPT_Intensity);
    }
    
    void BaseLight::setGlobalPosition(const glm::vec3& position)
    {
        m_oGlobalPosition = position;
        setDirty(LightPropertyType::LPT_GlobalPosition);
    }

    void BaseLight::setDirty(LightPropertyType type)
    {
        BROWSER_SET_BIT(m_uPropertiesDirty, type);
        dispatchEventToSystem(SystemType::Light, ComponentEvent::Light_UpdateLight, new UpdateLightMessage(this));
    }
    
	void BaseLight::handleEvent(ComponentEvent event, BaseComponentMessage* msg)
	{
//		switch (event)
//		{
//		case ComponentEvent::Transform_AddComponent:
//			{
////                BROWSER_LOG("Transform_AddComponent Message received");
//
//            }
//			break;
//
//        case ComponentEvent::Light_AddComponent:
//            {
//
//            }
//            break;
//		}
	}

	
    
}
