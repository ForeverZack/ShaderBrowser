#include "AnimationSystem.h"
#include "Browser/Components/Animation/Animator.h"

namespace browser
{
	AnimationSystem::AnimationSystem()
	{
		m_iPriority = 0;
		m_eSystemType = common::SystemType::Animation;
		m_bComponentMutex = true;
	}
    
    void AnimationSystem::update(float deltaTime)
    {
        Animator* animator = nullptr;
        for (auto itor = m_mComponentsList.cbegin(); itor != m_mComponentsList.cend(); ++itor)
        {
            const std::list<BaseComponent*>& animators = itor->second;
            animator = dynamic_cast<Animator*>(*(animators.begin()));
            animator->updateAnimation(deltaTime);
        }
    }

}
