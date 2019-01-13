#include "Reference.h"
#include "../System/AutoReleasePool.h"
#include "../Tools/Utils.h"

namespace common
{

	Reference::Reference()
		: m_iRefenceCount(0)
	{

	}

	Reference::~Reference()
	{
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
