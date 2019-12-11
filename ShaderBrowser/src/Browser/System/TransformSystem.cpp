#include "TransformSystem.h"
#include "GL/GLDefine.h"

namespace browser
{
	TransformSystem::TransformSystem()
        : m_oScene(nullptr)
	{
		m_iPriority = 0;
		m_eSystemType = common::SystemType::Transform;
		m_bComponentMutex = true;
        
        // 线程池
		m_pThread = new BaseThread();
	}

	void TransformSystem::init()
	{
        m_oScene = nullptr;
	}
    
    void TransformSystem::setScene(BaseEntity* scene)
    {
		BROWSER_ASSERT(scene->getComponent<Transform>(), "You cannot set the BaseEntity without Transform component as Scene in function TransformSystem::setScene");

        if(m_oScene)
        {
            m_oScene->release();
        }
        
        m_oScene = scene;
        m_oScene->retain();
    }
    
    void TransformSystem::beforeUpdate(float deltaTime)
    {
        for(auto itor = m_mComponentsList.begin(); itor!=m_mComponentsList.end(); ++itor)
        {
            const std::list<BaseComponent*>& transList = itor->second;
            static_cast<Transform*>(*(transList.begin()))->beforeUpdate(deltaTime);
        }
        // begin
        m_bIsFinish = false;
    }

	void TransformSystem::update(float deltaTime)
	{
		m_pThread->addTask(std::bind(&TransformSystem::updateScene, this));
	}

    void TransformSystem::updateScene()
    {
        if (m_oScene)
        {
            Transform* scene_transform = m_oScene->getComponent<Transform>();
            // 从scene节点开始遍历，更新每个节点的transform
            scene_transform->visit(customGL::GLM_MAT4_UNIT, false);
        }
        // finish
        m_bIsFinish = true;
    }



}
