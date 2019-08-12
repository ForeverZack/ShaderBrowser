#include "FeedbackSystem.h"

namespace browser
{
	FeedbackSystem::FeedbackSystem()
	{
		m_iPriority = 0;
		//m_eSystemType = common::SystemType::TransformFeedback;
		m_bComponentMutex = true;
	}
    
    void FeedbackSystem::update(float deltaTime)
    {
        
    }

}
