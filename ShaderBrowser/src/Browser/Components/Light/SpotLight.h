#pragma once

#include <vector>
#include "Browser/Components/Light/BaseLight.h"
using namespace common;

namespace browser
{
	class SpotLight : public BaseLight
	{
    public:
        static SpotLight* create(const std::string& name, BaseEntity* parent = nullptr);
        
	public:
		SpotLight();
		~SpotLight();

	public:
		// 刷新
		virtual void updateLight();
		// 更新材质的Light数据
		virtual void updateMaterialsLight(const std::unordered_map<unsigned int, Material*>& materials, unsigned int index = 0, bool forceUpdate = false);
		// 数据是否发生改变
		virtual bool isLightDirty();
		// 光源系统是否需要更新
		virtual bool isLightSystemDirty();

	public:
		REGISTER_PROPERTY_GET(float, m_fRange, Range)
		REGISTER_PROPERTY_GET(float, m_fSpotAngle, SpotAngle)
		// 设置光照范围
		void setRange(float range);
		// 设置光照广角
		void setSpotAngle(float angle);

	protected:
		// 光照范围
		float m_fRange;
		// 光照广角 (<180度)
		float m_fSpotAngle;
		// 光照衰减纹理 (基于距离)
		customGL::Texture2D* m_pLightTexture0;
		// 光照衰减纹理2 (基于广角)
		customGL::Texture2D* m_pLightTextureB0;

       // 阴影属性等
        
	};
}

