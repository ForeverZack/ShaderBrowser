#include "BoundBoxSystem.h"
#include "Browser/Components/BoundBox/BaseBoundBox.h"

namespace browser
{
	BoundBoxSystem::BoundBoxSystem()
	{
		m_iPriority = 0;
		m_eSystemType = common::SystemType::BoundBox;
		m_bComponentMutex = true;
	}

	void BoundBoxSystem::update(float deltaTime)
	{
		BaseBoundBox* boundBox = nullptr;
		for (auto itor = m_mComponentsList.cbegin(); itor != m_mComponentsList.cend(); ++itor)
		{
			const std::list<BaseComponent*>& boundBoxes = itor->second;
			boundBox = static_cast<BaseBoundBox*>(*(boundBoxes.begin()));
			boundBox->updateBoundBox(deltaTime);
		}
	}

}
