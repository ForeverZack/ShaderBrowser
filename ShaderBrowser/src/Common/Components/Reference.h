#ifndef COMMON_COMS_REFERENCE_H
#define COMMON_COMS_REFERENCE_H

#include "Common/Tools/Utils.h"

namespace common
{
	class AutoReleasePool;

	/*
		autorelease是给逻辑线程堆内存中的对象使用的，但c++没有方法可以准确地判断对象是在栈上还是堆上，所以需要加个标记去检测对象是否被提前释放(栈上的对象，或者提前手动release都会触发)。
		渲染命令中会使用到的数据对象(指命令中对原始数据对象的拷贝)，不应该调用autorelease。
	*/
	class Reference
	{
	public:
		Reference();
		virtual ~Reference();
		friend class AutoReleasePool;

        // 克隆对象(注意如果要使用clone就必须在子类中实现它!!)
        virtual Reference* clone();
        
	public:
		// 引用计数+1
		void retain();
		// 引用计数-1，并在引用计数=0的时候立即释放
		virtual void release();
		// 将refence放入AutoReleasePool里，并在每帧最后执行一次释放 (注意！！！仅可在逻辑线程调用！！！)
		void autorelease();


		REGISTER_PROPERTY_GET(int, m_iRefenceCount, RefenceCount)


	protected:
		// 对象是否在堆内存上
		bool m_bHeapMemory;
		// 引用计数
		int m_iRefenceCount;
		// 是否被加入待释放对象池 (Reference对象可能会被创建在栈上，或者手动release，这样都会提前释放被加入AutoReleasePool的对象，这些需要从AutoReleasePool中移除)
		int m_iAutoRelease;
	};
}

#endif
