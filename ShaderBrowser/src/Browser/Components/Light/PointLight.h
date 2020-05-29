#pragma once

#include <vector>
#include "Browser/Components/Light/BaseLight.h"
#include "GL/GPUResource/Texture/Texture2D.h"
using namespace common;

namespace browser
{
	class PointLight : public BaseLight
	{
    public:
        static PointLight* create(const std::string& name, BaseEntity* parent = nullptr);
        
	public:
		PointLight();
		~PointLight();

	public:
		// 刷新
		virtual void updateLight();
		// 更新材质的Light数据
		virtual void updateMaterialsLight(const std::unordered_map<unsigned int, Material*>& materials, unsigned int index = 0, bool forceUpdate = false);
		// 数据是否发生改变
		virtual bool isLightDirty();
		// 光源系统是否需要更新
		virtual bool isLightSystemDirty();
        
	private:
		// 重载属性面板显示方法
		virtual void onInspectorGUI(InspectorPanel* inspector);
		// 转换数据
		virtual void serialize();

    public:
        REGISTER_PROPERTY_GET(float, m_fRange, Range)
        // 设置光照范围
        void setRange(float range);
        
	protected:
        // 光照范围
        float m_fRange;
        // 光照衰减纹理
        customGL::Texture2D* m_pLightTexture0;
        // 阴影属性等
        
	};
}

