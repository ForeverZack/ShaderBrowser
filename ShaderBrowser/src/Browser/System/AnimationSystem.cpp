#include "AnimationSystem.h"
#include "Browser/Components/Animation/Animator.h"

namespace browser
{
	AnimationSystem::AnimationSystem()
	{
		m_iPriority = 0;
		m_eSystemType = common::SystemType::Animation;
		m_bComponentMutex = true;
        
        // 动画线程池
        m_pAnimThreadPool = new BaseThreadPool(MAX_ANIMATION_THREAD_COUNT);
	}

    AnimationSystem::~AnimationSystem()
    {
        delete m_pAnimThreadPool;
    }
    
    void AnimationSystem::update(float deltaTime)
    {
        Animator* animator = nullptr;
        for (auto itor = m_mComponentsList.cbegin(); itor != m_mComponentsList.cend(); ++itor)
        {
            const std::list<BaseComponent*>& animators = itor->second;
            animator = dynamic_cast<Animator*>(*(animators.begin()));
            
            m_pAnimThreadPool->addTask(std::bind(&Animator::updateAnimation, animator, deltaTime));
//            animator->updateAnimation(deltaTime);
        }
        
        // 等待线程执行完成
        while(m_pAnimThreadPool->getIsActive());
    }

}
