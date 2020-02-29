#pragma once

#include <vector>
#include "Browser/Components/Light/BaseLight.h"
using namespace common;

namespace browser
{
	class DirectionalLight : public BaseLight
	{
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
        
        // 设置平行光方向
        void setLightDirection(const glm::vec3& dir);
        void setLightDirection(float x, float y, float z);
  
    private:
        // 重载属性面板显示方法
        virtual void onInspectorGUI(InspectorPanel* inspector);
        
	protected:
        // 平行光方向
        glm::vec3 m_oLightDirection;
        // 阴影属性等
    
	};
}

