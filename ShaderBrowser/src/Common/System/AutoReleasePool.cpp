#include "AutoReleasePool.h"

namespace common
{
	AutoReleasePool::AutoReleasePool()
	{	
		// Vector如果超过其存储范围的话，会申请一块新的内存，并将所有现有的数据移动过去，这是一个很费时的过程，
		// 所以在一开始的时候，尽量能设置好vector的大小。
		m_vReleaseReferences.reserve(150);
		m_vReleaseReferences.clear();
        
        m_qRenderReferences.clear();
    }

	AutoReleasePool::~AutoReleasePool()
	{

	}

	void AutoReleasePool::addReference(Reference* ref)
	{
		++ref->m_iAutoRelease;
		m_vReleaseReferences.push_back(ref);
	}
    
    void AutoReleasePool::addReferenceFromRenderCore(Reference* ref)
    {
        m_qRenderReferences.push(ref);
    }

	void AutoReleasePool::removeReference(Reference* ref)
	{
		for (auto itor = m_vReleaseReferences.begin(); itor != m_vReleaseReferences.end(); ++itor)
		{
			if (*itor == ref)
			{
				(*itor) = nullptr;
			}
		}
	}

	void AutoReleasePool::update()
	{
		// 释放逻辑线程的对象
		releaseReferencesFromLogicCore();
		// 释放渲染线程的对象
		releaseReferencesFromRenderCore();
	}

	void AutoReleasePool::releaseReferencesFromLogicCore()
	{
		Reference* reference = nullptr;
		for (auto itor = m_vReleaseReferences.begin(); itor != m_vReleaseReferences.end(); ++itor)
		{
			reference = *itor;
			if (reference)
			{
				--reference->m_iAutoRelease;
				BROWSER_ASSERT(reference->m_iAutoRelease >= 0, " m_iAutoRelease must bigger then 0");
				reference->release();
			}
		}
		m_vReleaseReferences.clear();
	}

	void AutoReleasePool::releaseReferencesFromRenderCore()
	{
		// 将渲染线程的待释放对象加入逻辑线程队列
		m_qRenderReferences.operateQueue([&](std::queue<Reference*>& queue)
		{
			int length = queue.size();
			for (int i = 0; i < length; ++i)
			{
				m_vReleaseReferences.push_back(queue.front());
				queue.pop();
			}
		});

		Reference* reference = nullptr;
		for (auto itor = m_vReleaseReferences.begin(); itor != m_vReleaseReferences.end(); ++itor)
		{
			reference = *itor;
			BROWSER_SAFE_RELEASE_REFERENCE(reference);
		}
		m_vReleaseReferences.clear();
	}


}
