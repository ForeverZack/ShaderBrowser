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
	}

	void TransformSystem::init()
	{
        m_oScene = nullptr;
	}
    
    void TransformSystem::setScene(BaseEntity* scene)
    {
		BROWSER_ASSERT(scene->getTransform(), "You cannot set the BaseEntity without Transform component as Scene in function TransformSystem::setScene");

        if(m_oScene)
        {
            m_oScene->release();
        }
        
        m_oScene = scene;
        m_oScene->retain();
    }

	void TransformSystem::update(float deltaTime)
	{
		if (m_oScene)
        {
			Transform* scene_transform = m_oScene->getTransform();
            // 从scene节点开始遍历，更新每个节点的transform
			scene_transform->visit(customGL::GLM_MAT4_UNIT, false);
        }
	}




}
