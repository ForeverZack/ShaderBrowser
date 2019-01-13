#include "TransformSystem.h"
#include "../../GL/GLDefine.h"

namespace browser
{
	static TransformSystem* m_oInstance = nullptr;

	TransformSystem* TransformSystem::getInstance()
	{
		if (!m_oInstance)
		{
			m_oInstance = new TransformSystem();
		}
		return m_oInstance;
	}

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

	void TransformSystem::update(float deltaTime)
	{
		if (m_oScene)
        {
            // 从scene节点开始遍历，更新每个节点的transform
            m_oScene->visit(customGL::GLM_MAT4_UNIT, false);
        }
	}




}
