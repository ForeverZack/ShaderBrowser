#ifndef COMMON_SYS_AUTORELEASEPOOL_H
#define COMMON_SYS_AUTORELEASEPOOL_H

#include <vector>
#include "../Components/Reference.h"
#include "../Tools/BaseSingleton.h"

namespace common
{
	class AutoReleasePool : public BaseSingleton<AutoReleasePool>
	{
	public:
		AutoReleasePool();
		~AutoReleasePool();

	public:

		// 添加待释放的对象
		void addReference(Reference* ref);

		// 每帧结束时调用刷新，用来对队列中所有的refence对象进行一次release操作
		void update();

	private:
		std::vector<Reference*> m_vReleaseRefences;
	};
}

#endif
