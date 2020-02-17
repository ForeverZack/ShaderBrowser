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
		~AnimationSystem();

    public:
        // 每帧刷新
        void update(float deltaTime);

		// 增加完成计算的数量
		void dispatchOneFinishedComponent()
		{
			m_uFinishedCalCount = m_uFinishedCalCount.getValue() + 1;
		}
        
	private:
        // 线程池
        BaseThreadPool* m_pAnimThreadPool;
		// 等待计算的动画组件数量
		unsigned int m_uWaitCalCount;
		// 完成计算的动画组件数量
		MutexVariable<unsigned int> m_uFinishedCalCount;
	};
}
