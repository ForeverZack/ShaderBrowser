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
		m_vReleaseReferences.push_back(ref);
	}
    
    void AutoReleasePool::addReferenceFromRenderCore(Reference* ref)
    {
        m_qRenderReferences.push(ref);
    }

	void AutoReleasePool::update()
	{
        // 将渲染线程的待释放对象加入逻辑线程队列
        m_qRenderReferences.operateQueue([&](std::queue<Reference*>& queue)
        {
            for (int i=0; i<queue.size(); ++i)
            {
                m_vReleaseReferences.push_back(queue.front());
                queue.pop();
            }
        });
    
        
		std::vector<Reference*>::iterator iter = m_vReleaseReferences.begin();
		for(; iter!=m_vReleaseReferences.end(); ++iter)
		{
			(*iter)->release();
		}
		m_vReleaseReferences.clear();
	}


}
