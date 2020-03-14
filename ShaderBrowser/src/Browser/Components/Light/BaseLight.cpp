#include "BaseLight.h"
#include "Browser/Entitys/BaseEntity.h"

namespace browser
{
    // uniform变量名称
    // 平行光
    // 平行光颜色
    const char* BaseLight::SHADER_UNIFORM_DIRECTIONAL_COLOR = "CGL_DIRECTIONAL_LIGHTS[%d].color";
    // 平行光强度
    const char* BaseLight::SHADER_UNIFORM_DIRECTIONAL_INTENSITY = "CGL_DIRECTIONAL_LIGHTS[%d].intensity";
    // 平行光方向
    const char* BaseLight::SHADER_UNIFORM_DIRECTIONAL_DIRECTION = "CGL_DIRECTIONAL_LIGHTS[%d].direction";
    // 平行光光源矩阵
	const char* BaseLight::SHADER_UNIFORM_DIRECTIONAL_LIGHTMATRIX = "CGL_DIRECTIONAL_LIGHTS[%d].lightMatrix";
    // 点光源
    // 点光源颜色
    const char* BaseLight::SHADER_UNIFORM_POINT_COLOR = "CGL_POINT_LIGHTS[%d].color";
    // 点光源强度
    const char* BaseLight::SHADER_UNIFORM_POINT_INTENSITY = "CGL_POINT_LIGHTS[%d].intensity";
    // 点光源位置
    const char* BaseLight::SHADER_UNIFORM_POINT_POSITION = "CGL_POINT_LIGHTS[%d].position";
    // 点光源光源矩阵
    const char* BaseLight::SHADER_UNIFORM_POINT_LIGHTMATRIX = "CGL_POINT_LIGHTS[%d].lightMatrix";
	// 聚光灯
	// 聚光灯颜色
	const char* BaseLight::SHADER_UNIFORM_SPOT_COLOR = "CGL_SPOT_LIGHTS[%d].color";
	// 聚光灯强度
	const char* BaseLight::SHADER_UNIFORM_SPOT_INTENSITY = "CGL_SPOT_LIGHTS[%d].intensity";
	// 聚光灯位置
	const char* BaseLight::SHADER_UNIFORM_SPOT_POSITION = "CGL_SPOT_LIGHTS[%d].position";
	// 聚光灯光源矩阵
	const char* BaseLight::SHADER_UNIFORM_SPOT_LIGHTMATRIX = "CGL_SPOT_LIGHTS[%d].lightMatrix";
    
	BaseLight::BaseLight()
        : BaseComponent("Light")
        , m_oColor(GLM_COLOR_WHITE)
        , m_fIntensity(1.0f)
		, m_oLightDirection(GLM_AXIS_X)
		, m_oLightMatrix(GLM_MAT4_UNIT)
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
			const glm::vec3& globalPosition = transform->getGlobalPosition();
			if (m_oGlobalPosition != globalPosition)
			{
				recordGlobalPosition(globalPosition);
			}
			const glm::vec3& forward = transform->getForward();
			if (m_oLightDirection != forward)
			{
				recordDirection(forward);
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
        setColor(color.r, color.g, color.b, color.a);
    }
    
    void BaseLight::setColor(float r, float g, float b, float a)
    {
        m_oColor.r = r;
        m_oColor.g = g;
        m_oColor.b = b;
        m_oColor.a = a;
        setDirty(LightPropertyType::LPT_Color);
    }
    
    void BaseLight::setIntensity(float intensity)
    {
        m_fIntensity = intensity;
        setDirty(LightPropertyType::LPT_Intensity);
    }
    
    void BaseLight::recordGlobalPosition(const glm::vec3& position)
    {
        m_oGlobalPosition = position;
        setDirty(LightPropertyType::LPT_GlobalPosition);
		setDirty(LightPropertyType::LPT_LightMatrix);
    }

	void BaseLight::recordDirection(const glm::vec3& direction)
	{
		m_oLightDirection = glm::normalize(direction);  // 标准化
		setDirty(LightPropertyType::LPT_Direction);
		setDirty(LightPropertyType::LPT_LightMatrix);
	}

    void BaseLight::setDirty(LightPropertyType type)
    {
        BROWSER_SET_BIT(m_uPropertiesDirty, type);
        dispatchEventToSystem(SystemType::Light, ComponentEvent::Light_UpdateLight, new UpdateLightMessage(this));
    }

	void BaseLight::setPosition(float x, float y, float z)
	{
		Transform* transform = m_oBelongEntity ? m_oBelongEntity->getComponent<Transform>() : nullptr;
		if (transform)
		{
			transform->setPosition(x, y, z);
			recordGlobalPosition(transform->getGlobalPosition());
		}
	}
	
	void BaseLight::setEulerAngle(float x, float y, float z)
	{
		Transform* transform = m_oBelongEntity ? m_oBelongEntity->getComponent<Transform>() : nullptr;
		if (transform)
		{
			transform->setEulerAngle(x, y, z);
			recordDirection(transform->getForward());
		}
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
