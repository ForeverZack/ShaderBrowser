#pragma once

#include <vector>
#include "Browser/Components/Light/BaseLight.h"
using namespace common;

namespace browser
{
	class BaseEntity;

	class DirectionalLight : public BaseLight
	{
	public:
		static DirectionalLight* create(const std::string& name, BaseEntity* parent = nullptr);

	public:
		DirectionalLight();
		~DirectionalLight();

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

	protected:
        // 阴影属性等
    
	};
}

