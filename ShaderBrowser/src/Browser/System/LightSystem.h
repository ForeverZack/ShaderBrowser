#pragma once

#include <vector>
#include "Common/System/BaseSystem.h"
#include "Common/Tools/BaseSingleton.h"
#include "Browser/Components/Light/BaseLight.h"

using namespace common;

namespace browser
{
    
	class LightSystem : public common::BaseSystem, public BaseSingleton<LightSystem>
	{
    public:
        // 光源修改类型
        enum LightChangeType
        {
            // 新增光源
            LCT_NewLight = 0,
            // 删除光源
            LCT_DeleteLight,
            // 光源强度发生改变
            LCT_Intensity,
         
            // 平行光数据发生改变
            LCT_Directional,
        };
        
	public:
		LightSystem();
		~LightSystem() {};

    public:
        // 添加组件
        virtual bool addComponent(BaseEntity* entity, BaseComponent* component);
        // 移除组件
        virtual bool removeComponent(BaseEntity* entity, BaseComponent* component);
        // 每帧刷新
        virtual void update(float deltaTime);
        // 在系统刷新后（用来做一些后处理）
        virtual void afterUpdate(float deltaTime);
    
    public:
        // 更新材质光源属性
        void updateMaterialLights(std::unordered_map<std::string, UniformValue>& uniforms);
        
    protected:
        // 接受事件
        virtual void handleEvent(ComponentEvent event, BaseComponentMessage* msg);
        
	private:
        // 平行光列表
        std::vector<BaseLight*> m_vDirectionalLights;
        // 平行光列表脏标记
        unsigned int m_uDirectionalDirty;
        // 点光源列表
        std::vector<BaseLight*> m_vPointLights;
		// 点光源列表脏标记
		unsigned int m_uPointDirty;
        // 聚光灯列表
        std::vector<BaseLight*> m_vSpotLights;
		// 聚光灯列表脏标记
		unsigned int m_uSpotDirty;
	};
}
