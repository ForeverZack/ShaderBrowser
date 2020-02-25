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
        // 更新所有材质的Light数据
        virtual void updateAllMaterialsLight(unsigned int index = 0);
		// 更新指定材质的所有Light数据（强制更新所有属性）
		virtual void updateAllNewMaterialsLight(unsigned int index = 0);
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

