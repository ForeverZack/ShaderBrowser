#ifndef COMMON_COMS_REFERENCE_H
#define COMMON_COMS_REFERENCE_H

#include "../Tools/Utils.h"

namespace common
{
	class Reference
	{
	public:
		Reference();
		virtual ~Reference();

	public:
		// 引用计数+1
		void retain();
		// 引用计数-1，并在引用计数=0的时候立即释放
		void release();
		// 将refence放入AutoReleasePool里，并在每帧最后执行一次释放
		void autorelease();


		REGISTER_PROPERTY_GET(int, m_iRefenceCount, RefenceCount)

	private:
		// 引用计数
		int m_iRefenceCount;
	};
}

#endif
