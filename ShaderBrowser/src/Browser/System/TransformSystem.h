#pragma once

#include <vector>
#include "Common/Tools/Utils.h"
#include "Common/System/BaseSystem.h"
#include "Browser/Components/Transform/Transform.h"
#include "Common/Tools/BaseSingleton.h"
#include "Common/Tools/Thread/BaseThreadPool.h"

using namespace common;

namespace browser
{
    
	class TransformSystem : public common::BaseSystem, public BaseSingleton<TransformSystem>
	{
	public:
		TransformSystem();
		~TransformSystem() {};

		// init
		void init();
        
        // 每帧前刷新
        void beforeUpdate(float deltaTime);
		// 每帧更新
		void update(float deltaTime);
        
        // 设置场景节点
        void setScene(BaseEntity* scene);
        
		
		REGISTER_PROPERTY_GET(BaseEntity*, m_oScene, Scene)
        REGISTER_PROPERTY_GET(bool, m_bIsUpdateFinish, IsUpdateFinish)

    private:
        // 刷新整个场景
        void updateScene();
        
	private:
		// 场景根节点
        BaseEntity* m_oScene;
        // 是否更新完毕
        bool m_bIsUpdateFinish;
        // 线程
        BaseThreadPool* m_pThreadPool;
	};
}
