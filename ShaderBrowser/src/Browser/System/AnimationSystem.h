#pragma once

#include <vector>
#include "Common/System/BaseSystem.h"
#include "Common/Tools/BaseSingleton.h"
#include "Common/Tools/Thread/BaseThreadPool.h"

using namespace common;

namespace browser
{
    
	class AnimationSystem : public common::BaseSystem, public BaseSingleton<AnimationSystem>
	{
    // 计算骨骼动画最大线程数量
    #define MAX_ANIMATION_THREAD_COUNT 10
        
	public:
		AnimationSystem();
		~AnimationSystem() {};

    public:
        // 每帧刷新
        void update(float deltaTime);
        
	private:
        BaseThreadPool* m_pAnimThreadPool;
	};
}
