#include "AnimationSystem.h"
#include "Common/System/ECSManager.h"
#include "Browser/Components/Animation/Animator.h"
#include <chrono>

namespace browser
{
	AnimationSystem::AnimationSystem()
		: m_uFinishedCalCount(0)
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
		// 待计算的组件数量 TODO: 这里暂时是计算全部组件，后面可能会根据物体是否显示以及在不在视野范围内来确定待计算的数量
		m_uWaitCalCount = m_mComponentsList.size();
		m_uFinishedCalCount = 0;

        Animator* animator = nullptr;
        
		// 等TransformSystem更新完再计算，以免发生冲突
		//while (!ECSManager::getInstance()->isSystemFinish(SystemType::Transform));
        
        // 现在将骨骼动画的更新拆成了2步：1.计算骨骼变换，这个是模型自身的内部数据，不受其他外部节点变换的影响，所以可以先计算。2.计算所有骨骼节点的Transform，这里会受外部其他Transform节点的影响，所以要等TransformSystem执行完成才能执行。动画的更新拆成这2步，就是怕等待TransformSystem过程过久，造成线程资源的浪费
        // 1.计算骨骼变换
        for (auto itor = m_mComponentsList.cbegin(); itor != m_mComponentsList.cend(); ++itor)
        {
            const std::list<BaseComponent*>& animators = itor->second;
            animator = dynamic_cast<Animator*>(*(animators.begin()));
            
            m_pAnimThreadPool->addTask(std::bind(&Animator::updateAnimation, animator, deltaTime));
//            animator->updateAnimation(deltaTime);
        }
        
//        // 2.计算骨骼节点的Transform
//        for (auto itor = m_mComponentsList.cbegin(); itor != m_mComponentsList.cend(); ++itor)
//        {
//            const std::list<BaseComponent*>& animators = itor->second;
//            animator = dynamic_cast<Animator*>(*(animators.begin()));
//            
//            m_pAnimThreadPool->addTask(std::bind(&Animator::updateBonesTranform, animator));
////            animator->updateAnimation(deltaTime);
//        }
        
        // 等待线程执行完成
		while (m_uFinishedCalCount.getValue()<m_uWaitCalCount && m_pAnimThreadPool->getIsActive());
  //      while(m_pAnimThreadPool->getIsActive());

    }

}
