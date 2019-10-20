#include "BoundBoxSystem.h"
#include "Browser/Components/BoundBox/BaseBoundBox.h"

namespace browser
{
	BoundBoxSystem::BoundBoxSystem()
	{
		m_iPriority = 0;
		m_eSystemType = common::SystemType::BoundBox;
		m_bComponentMutex = true;
        
        // 包围盒线程池
        m_pBoundThreadPool = new BaseThreadPool(MAX_BOUND_THREAD_COUNT);
	}
    
    BoundBoxSystem::~BoundBoxSystem()
    {
        delete m_pBoundThreadPool;
    }

	void BoundBoxSystem::update(float deltaTime)
	{
        if (!m_pBoundThreadPool->getIsActive()) // 所有包围盒都刷完了，才刷下一遍
        {
            BaseBoundBox* boundBox = nullptr;
            for (auto itor = m_mComponentsList.cbegin(); itor != m_mComponentsList.cend(); ++itor)
            {
                const std::list<BaseComponent*>& boundBoxes = itor->second;
                boundBox = static_cast<BaseBoundBox*>(*(boundBoxes.begin()));
    //            boundBox->updateBoundBox(deltaTime);
                m_pBoundThreadPool->addTask(std::bind(&BaseBoundBox::updateBoundBox, boundBox, deltaTime));
            }
        }

	}

}
