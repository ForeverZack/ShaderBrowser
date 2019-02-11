#include "Reference.h"
#include "Common/System/AutoReleasePool.h"
#include "Common/Tools/Utils.h"

namespace common
{

	Reference::Reference()
		: m_iRefenceCount(0)
	{
	}

	Reference::~Reference()
	{
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
