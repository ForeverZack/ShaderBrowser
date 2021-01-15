#pragma once

#include <stdio.h>
#ifdef  _WIN32
#pragma warning(disable:4996)
#endif //  _WIN32
#include <vector>
#include "GL/GLDefine.h"
#include "Browser/Components/BaseComponent.h"
#include "Browser/Components/Transform/Transform.h"
#include "GL/GPUResource/Shader/Material.h"
#include "Common/Tools/Utils.h"
#include "Browser/Entitys/BaseEntity.h"

#include <glm/glm.hpp>

using namespace common;
using namespace customGL;

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
            LPT_Intensity,
            // 世界位置
            LPT_GlobalPosition,
            // 方向 (平行光、聚光灯)
            LPT_Direction,
			// 光源空间矩阵（世界空间->光源空间）
			LPT_LightMatrix,
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
        // 更新所有材质的Light数据
		virtual void updateMaterialsLight(const std::unordered_map<unsigned int, Material*>& materials, unsigned int index = 0, bool forceUpdate = false) {};
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
		REGISTER_PROPERTY_CONSTREF_GET(std::vector<glm::vec4>, m_vLightData, LightData)
        void setColor(const glm::vec4& color);
        void setColor(float r, float g, float b, float a);
        void setIntensity(float intensity);
		void setPosition(float x, float y, float z);
		void setEulerAngle(float x, float y, float z);
        
    protected:
        // 处理组件事件
        virtual void handleEvent(ComponentEvent event, BaseComponentMessage* msg);
        // 设置脏标记
        virtual void setDirty(LightPropertyType type);
		// 转换数据
		virtual void serialize() {};
        
		void recordDirection(const glm::vec3& direction);
        void recordGlobalPosition(const glm::vec3& position);

    protected:
        // uniform变量名称
        // 平行光
        // 平行光颜色
        static const char* SHADER_UNIFORM_DIRECTIONAL_COLOR;
        // 平行光强度
        static const char* SHADER_UNIFORM_DIRECTIONAL_INTENSITY;
        // 平行光方向
        static const char* SHADER_UNIFORM_DIRECTIONAL_DIRECTION;
		// 平行光光源矩阵
		static const char* SHADER_UNIFORM_DIRECTIONAL_LIGHTMATRIX;
        // 点光源
        // 点光源颜色
        static const char* SHADER_UNIFORM_POINT_COLOR;
        // 点光源强度
        static const char* SHADER_UNIFORM_POINT_INTENSITY;
        // 点光源位置
        static const char* SHADER_UNIFORM_POINT_POSITION;
        // 点光源光源矩阵
        static const char* SHADER_UNIFORM_POINT_LIGHTMATRIX;
		// 聚光灯
		// 聚光灯颜色
		static const char* SHADER_UNIFORM_SPOT_COLOR;
		// 聚光灯强度
		static const char* SHADER_UNIFORM_SPOT_INTENSITY;
		// 聚光灯位置
		static const char* SHADER_UNIFORM_SPOT_POSITION;
		// 聚光灯光源矩阵
		static const char* SHADER_UNIFORM_SPOT_LIGHTMATRIX;
        
	protected:
        // 光源类型
        LightType m_eType;
		// 光源数据
		std::vector<glm::vec4> m_vLightData;
        
        // 光源数据脏标记
        unsigned int m_uPropertiesDirty;
        // 光源颜色
        glm::vec4 m_oColor;
        // 强度
        float m_fIntensity;
		// 方向
		glm::vec3 m_oLightDirection;
        // 世界位置
        glm::vec3 m_oGlobalPosition;
		// 光源空间矩阵
		glm::mat4 m_oLightMatrix;
        
        // 阴影类型
        ShadowType m_eShadowType;
        
	};
}

