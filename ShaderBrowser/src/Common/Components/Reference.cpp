#include "Reference.h"
#include "Common/System/AutoReleasePool.h"
#include "Common/Tools/Utils.h"

namespace common
{

	Reference::Reference()
		: m_iRefenceCount(0)
		, m_iAutoRelease(0)
		, m_bHeapMemory(false)
	{
	}

	Reference::~Reference()
	{
		// 对象被提前释放(如在栈上的对象，或者自己手动释放)
		if (m_iAutoRelease > 0)
		{
			AutoReleasePool::getInstance()->removeReference(this);
		}
	}
    
    Reference* Reference::clone()
    {
        BROWSER_LOG("Reference has no clone function in child's class, you must check your stack in function Reference::clone");
        
        return nullptr;
    }

	void Reference::retain()
	{
		++m_iRefenceCount;
	}

	void Reference::release()
	{
		BROWSER_ASSERT(m_iRefenceCount>0, "Reference::release() : reference must bigger then 0");
		--m_iRefenceCount;
		if (m_iRefenceCount == 0)
		{
			delete this;
		}
	}

	void Reference::autorelease()
	{
        retain();
		AutoReleasePool::getInstance()->addReference(this);
	}


}
