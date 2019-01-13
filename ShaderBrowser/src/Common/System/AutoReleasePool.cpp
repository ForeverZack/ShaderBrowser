#include "AutoReleasePool.h"

namespace common
{
	static AutoReleasePool* m_oInstance = nullptr;
	AutoReleasePool* AutoReleasePool::getInstance()
	{
		if (!m_oInstance)
		{
			m_oInstance = new AutoReleasePool();
		}
		return m_oInstance;
	}

	AutoReleasePool::AutoReleasePool()
	{	
		// Vector如果超过其存储范围的话，会申请一块新的内存，并将所有现有的数据移动过去，这是一个很费时的过程，
		// 所以在一开始的时候，尽量能设置好vector的大小。
		m_vReleaseRefences.reserve(150);
		m_vReleaseRefences.clear();
	}

	AutoReleasePool::~AutoReleasePool()
	{

	}

	void AutoReleasePool::addReference(Reference* ref)
	{
		m_vReleaseRefences.push_back(ref);
	}

	void AutoReleasePool::update()
	{
		std::vector<Reference*>::iterator iter = m_vReleaseRefences.begin();
		for(; iter!=m_vReleaseRefences.end(); ++iter)
		{
			(*iter)->release();
		}
		m_vReleaseRefences.clear();
	}


}
