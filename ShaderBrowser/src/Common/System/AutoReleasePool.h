#ifndef COMMON_SYS_AUTORELEASEPOOL_H
#define COMMON_SYS_AUTORELEASEPOOL_H

#include <vector>
#include "Common/Components/Reference.h"
#include "Common/Tools/BaseSingleton.h"
#include "Common/Tools/Thread/BaseThread.h"

namespace common
{
	class AutoReleasePool : public BaseSingleton<AutoReleasePool>
	{
	public:
		AutoReleasePool();
		~AutoReleasePool();

	public:

		// 添加逻辑线程待释放的对象
		void addReference(Reference* ref);
        // 添加渲染线程代释放的对象
        void addReferenceFromRenderCore(Reference* ref);
        
		// 每帧结束时调用刷新，用来对队列中所有的refence对象进行一次release操作
		void update();

	private:
        // 待释放对象队列 (逻辑线程)
		std::vector<Reference*> m_vReleaseReferences;
        // 渲染线程的待释放对象队列
        MutexQueue<Reference*> m_qRenderReferences;
    };
}

#endif
