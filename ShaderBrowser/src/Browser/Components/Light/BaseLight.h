#pragma once

#include <vector>
#include "Browser/Components/BaseComponent.h"
#include "Browser/Components/Transform/Transform.h"
#include "Common/Tools/Utils.h"

#include <glm/glm.hpp>

using namespace common;

namespace browser
{
	class BaseLight : public BaseComponent
	{
    public:
        // 包围盒类型
        enum LightType
        {
            // 平行光
            Directional = 0,
            // 点光源
            Point,
            // 聚光灯
            Spot,
        };
        
        // 阴影类型
        enum ShadowType
        {
            // 不产生阴影
            None = 0,
            // 硬阴影
            Hard,
            // 软阴影
            Soft,
        };
        
	public:
		BaseLight();
		virtual ~BaseLight();

	public:
		
        
        REGISTER_PROPERTY_GET(LightType, m_eType, LightType)
        REGISTER_PROPERTY_CONSTREF_GET(glm::vec4, m_oColor, Color)
        REGISTER_PROPERTY_SET(glm::vec4, m_oColor, Color)
        REGISTER_PROPERTY_GET_SET(float, m_fIntensity, Intensity)
        
    protected:
        // 处理组件事件
        virtual void handleEvent(ComponentEvent event, BaseComponentMessage* msg);
        
	protected:
        // 光源类型
        LightType m_eType;
        // 光源颜色
        glm::vec4 m_oColor;
        // 强度
        float m_fIntensity;
        
        // 阴影类型
        ShadowType m_eShadowType;
        
	};
}

