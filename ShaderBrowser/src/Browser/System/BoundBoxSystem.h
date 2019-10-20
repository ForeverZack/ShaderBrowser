#pragma once

#include <vector>
#include "Common/System/BaseSystem.h"
#include "Common/Tools/BaseSingleton.h"
#include "Common/Tools/Thread/BaseThreadPool.h"

using namespace common;

namespace browser
{
	class BoundBoxSystem : public common::BaseSystem, public BaseSingleton<BoundBoxSystem>
	{
    // 计算包围盒最大线程数量
    #define MAX_BOUND_THREAD_COUNT 10
        
	public:
		BoundBoxSystem();
		~BoundBoxSystem();

	public:
		// 每帧刷新
		void update(float deltaTime);


	private:
        // 线程池
        BaseThreadPool* m_pBoundThreadPool;
	};
}
