#pragma once

#include <stdio.h>
#ifdef  _WIN32
#pragma warning(disable:4996)
#endif //  _WIN32
#include <vector>
#include "GL/GLDefine.h"
#include "Browser/Components/BaseComponent.h"
#include "Browser/Components/Transform/Transform.h"
#include "Common/Tools/Utils.h"
#include "Common/System/Cache/MaterialCache.h"

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
        
        // 灯光属性枚举(用来控制脏标记)
        enum LightPropertyType
        {
            // 光源颜色
            LPT_Color = 0,
            // 强度
            LPT_Intensity = 1,
            // 世界位置
            LPT_GlobalPosition = 2,
            // 平行光方向
            LPT_LightDirection = 2,
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
        // 刷新
        virtual void updateLight();
        // 更新所有材质的Light数据（只更新改变的属性）
		virtual void updateAllMaterialsLight(unsigned int index = 0) {};
		// 更新所有新增材质的Light数据（强制更新所有属性）
		virtual void updateAllNewMaterialsLight(unsigned int index = 0) {};
        // 数据是否发生改变
        virtual bool isLightDirty();
        // 光源系统是否需要更新
        virtual bool isLightSystemDirty();
        // 重置light脏标记
        void resetLightDirty();
        
        REGISTER_PROPERTY_GET(LightType, m_eType, LightType)
        REGISTER_PROPERTY_CONSTREF_GET(glm::vec4, m_oColor, Color)
        REGISTER_PROPERTY_GET(float, m_fIntensity, Intensity)
        REGISTER_PROPERTY_CONSTREF_GET(glm::vec3, m_oGlobalPosition, GlobalPosition)
        void setColor(const glm::vec4& color);
        void setIntensity(float intensity);
        
    protected:
        // 处理组件事件
        virtual void handleEvent(ComponentEvent event, BaseComponentMessage* msg);
        // 设置脏标记
        virtual void setDirty(LightPropertyType type);
        
        void setGlobalPosition(const glm::vec3& position);
        
    protected:
        // uniform变量名称
        // 平行光颜色
        static const char* SHADER_UNIFORM_DIRECTIONAL_COLOR;
        // 平行光强度
        static const char* SHADER_UNIFORM_DIRECTIONAL_INTENSITY;
        // 平行光方向
        static const char* SHADER_UNIFORM_DIRECTIONAL_DIRECTION;
        
	protected:
        // 光源类型
        LightType m_eType;
        
        // 光源数据脏标记
        unsigned int m_uPropertiesDirty;
        // 光源颜色
        glm::vec4 m_oColor;
        // 强度
        float m_fIntensity;
        // 世界位置
        glm::vec3 m_oGlobalPosition;
        
        // 阴影类型
        ShadowType m_eShadowType;
        
	};
}

